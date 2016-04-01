/*  MINI GAUSSSENSE - ARDUINO MODE (VER. 2)
    Copyright (C) <2016> <GAUSSTOYS INC., TAIWAN (http://gausstoys.com)>
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "BicubicInterpolator.h"
#include "BiLinearInterpolator.h"
#include "GData.h"

/*If using 1 GaussSense*/
//const int MINI_GAUSSSENSE_AMOUNT = 1;     
//const int GS_ARRAY_W = 1;
//const int GS_ARRAY_H = 1;
//const int UPSAMPLE_RATE = 3;

/*If using 2x2 GaussSense Shield*/
const int MINI_GAUSSSENSE_AMOUNT = 4;     
const int GS_ARRAY_W = 2;
const int GS_ARRAY_H = 2;
const int UPSAMPLE_RATE = 1;

const int analogInPin[] = {A0, A1, A2, A3};       //Set the analog pins used
const int selectionPin[]  = {2, 3, 4, 5};  //Set the selection pins used

const int RAW_W = 4 * GS_ARRAY_W;
const int RAW_H = 4 * GS_ARRAY_H;
const int SAMPLE_W = UPSAMPLE_RATE * (RAW_W - 1);
const int SAMPLE_H = UPSAMPLE_RATE * (RAW_H - 1);

int sensorVal[RAW_W][RAW_H];
int initSensorVal[RAW_W][RAW_H];
int sampleVal[SAMPLE_W][SAMPLE_H];

BicubicInterpolator bcp;
BiLinearInterpolator blp;

int initiated = false;

int NOISE_THLD = 10;
int COORDINATE_W = 400;
int COORDINATE_H = 400;

GData basicNorth = GData(-1, -1, -1);
GData basicSouth = GData(-1, -1, -1);
GData basicBipolar = GData(-1, -1, -1);

void setup()
{
  Serial.begin(115200);
  for (int i = 0 ; i < 4 ; i++) pinMode(selectionPin[i], OUTPUT);
}

void loop()
{
  getGaussSenseData();  // Get data from all GaussSense modules
  if (!initiated) {
    initSensor();
    initiated = true;
  } else {
    upSampling(UPSAMPLE_RATE);
//    printRawData();
//    printUpsampledData();

    basicNorth = getBasicNorthPoint(COORDINATE_W, COORDINATE_H);
    basicSouth = getBasicSouthPoint(COORDINATE_W, COORDINATE_H);
    basicBipolar = getBasicBipolarPoint(basicNorth, basicSouth, COORDINATE_W, COORDINATE_H);
    
    Serial.print("N:");    
    printGData(basicNorth);
//    Serial.print((int)(basicNorth.getIntensity() * 200 / 127));
    Serial.print("S:");
    printGData(basicSouth);
//    Serial.print((int)(basicSouth.getIntensity() * 200 / 127));
    Serial.print("B:");
    printGData(basicBipolar);
    Serial.print(" Roll:");
    Serial.print((int)basicBipolar.getAngle());
    Serial.print(" Tilt:");
    Serial.print((int)basicBipolar.getPitch());
    Serial.print('\n');
  }
}

void initSensor() {
  for (int x = 0; x < 16; x++) {
    for (int n = 0; n < MINI_GAUSSSENSE_AMOUNT ; n++) {
      int ix = (x % 4) + (n%GS_ARRAY_W)*4;
      int iy = (x / 4) + (n/GS_ARRAY_H)*4;
      initSensorVal[ix][iy] = sensorVal[ix][iy];
    }
  }
}

void getGaussSenseData() { //Function for getting data from all GaussSense modules
  int muxBits[4];
  for (int x = 0; x < 16; x++) {
    for (int i = 0; i < 4; i++) {
      muxBits[i] = (x >> i) & 0x01;
      digitalWrite(selectionPin[i], muxBits[i]);
    }
    for (int n = 0; n < MINI_GAUSSSENSE_AMOUNT ; n++) {
      int ix = (x % 4) + (n%GS_ARRAY_W)*4;
      int iy = (x / 4) + (n/GS_ARRAY_H)*4;
      int v = analogRead(analogInPin[n]) - 512;
      sensorVal[ix][iy] = constrain((v - initSensorVal[ix][iy]) , -127, 127);
    }
  }
}

GData getBasicBipolarPoint(GData gN, GData gS, int w, int h) {
  GData gB = GData(-1, -1, -1, 0, 0);
  if (gN.getIntensity() > NOISE_THLD || gS.getIntensity() > NOISE_THLD) {
    float midX = (gN.getX() + gS.getX())/2;
    float midY = (gN.getY() + gS.getY())/2;
    float midIntensity = (gN.getIntensity() + gS.getIntensity())/2;
    float midRoll = degrees(atan2(gN.getY() - gS.getY(), (gN.getX() - gS.getX())));
    float midTilt = gN.getIntensity() - gS.getIntensity();
    gB.set(midX, midY, midIntensity, midRoll, midTilt);
  }
  return gB;
}

GData getBasicNorthPoint(int w, int h) {
  GData g = GData(-1, -1, -1);
  float weighted_x = 0;
  float weighted_y = 0;
  float maxIntensity = 0;
  float totalWeights = 0;
  for (int y = 0; y < SAMPLE_H; y++) {
    for (int x = 0; x < SAMPLE_W; x++) {
      float intensity = (float)sampleVal[x][y];
      if (intensity > NOISE_THLD) {
        weighted_x += intensity * x;
        weighted_y += intensity * y;
        totalWeights += intensity;
        if (intensity > maxIntensity) maxIntensity = intensity;
      }
    }
  }
  weighted_x = (weighted_x / totalWeights) * (w / (SAMPLE_W - 1));
  weighted_y = (weighted_y / totalWeights) * (h / (SAMPLE_H - 1));
  g.set(weighted_x, weighted_y, maxIntensity);
  return g;
}

GData getBasicSouthPoint(int w, int h) {
  GData g = GData(-1, -1, -1);
  float weighted_x = 0;
  float weighted_y = 0;
  float minIntensity = 0;
  float totalWeights = 0;
  for (int y = 0; y < SAMPLE_H; y++) {
    for (int x = 0; x < SAMPLE_W; x++) {
      float intensity = (float)sampleVal[x][y];
      if (intensity < -NOISE_THLD) {
        weighted_x += (-intensity) * x;
        weighted_y += (-intensity) * y;
        totalWeights += (-intensity);
        if (intensity < minIntensity) minIntensity = intensity;
      }
    }
  }
  weighted_x = (weighted_x / totalWeights) * (w / (SAMPLE_W - 1));
  weighted_y = (weighted_y / totalWeights) * (h / (SAMPLE_H - 1));
  g.set(weighted_x, weighted_y, minIntensity);
  return g;
}

void printGData(GData g) {
  Serial.print("(x,y) = (");
  Serial.print((int)g.getX());
  Serial.print(",");
  Serial.print((int)g.getY());
  Serial.print(") ");
  Serial.print((int)(g.getIntensity() * 200 / 127));
  Serial.print(" Gauss;");
}

void printRawData() { //Function for writing data to the serial port
  for (int y = 0; y < RAW_H; y++) {
    for (int x = 0; x < RAW_W; x++) {
      Serial.print(sensorVal[x][y]);
      Serial.print("\t");
      if (x % RAW_W == (RAW_W - 1)) Serial.print("\n");
    }
  }
  Serial.print("====\n");
}


void printUpsampledData() { //Function for writing data to the serial port
  for (int y = 0; y < SAMPLE_H; y++) {
    for (int x = 0; x < SAMPLE_W; x++) {
      Serial.print(sampleVal[x][y]);
      Serial.print("\t");
      if (x % SAMPLE_W == (SAMPLE_W - 1)) Serial.print("\n");
    }
  }
  Serial.print("====\n");
}

void upSampling(int ratio) {
  float pBC[4][4];
  float pBL[2][2];

  //BiLinear Interpolate the left and right edge
  for (int i = 0; i < RAW_W; i += RAW_W - 2) {
    for (int j = 1; j < RAW_H - 2; j++) {
      pBL[0][0] = sensorVal[i    ][j];  pBL[0][1] = sensorVal[i    ][j + 1];
      pBL[1][0] = sensorVal[i + 1][j];  pBL[1][1] = sensorVal[i + 1][j + 1];
      blp.updateCoefficients(pBL);
      recordSampleData(true, ratio, i, j); //true: isBoundary
    }
  }
  //BiLinear Interpolate the top and bottom edge
  for (int i = 0; i < RAW_W - 1; i++) {
    for (int j = 0; j < RAW_H; j += RAW_H - 2) {
      pBL[0][0] = sensorVal[i    ][j];  pBL[0][1] = sensorVal[i    ][j + 1];
      pBL[1][0] = sensorVal[i + 1][j];  pBL[1][1] = sensorVal[i + 1][j + 1];
      blp.updateCoefficients(pBL);
      recordSampleData(true, ratio, i, j); //true: isBoundary
    }
  }
  //Bicubic Interpolate other points
  for (int i = 1; i < RAW_W - 2; i++) {
    for (int j = 1; j < RAW_H - 2; j++) {
      pBC[0][0] = sensorVal[(i - 1)][j - 1];  pBC[0][1] = sensorVal[(i - 1)][j    ];
      pBC[0][2] = sensorVal[(i - 1)][j + 1];  pBC[0][3] = sensorVal[(i - 1)][j + 2];
      pBC[1][0] = sensorVal[(i)    ][j - 1];  pBC[1][1] = sensorVal[(i)    ][j    ];
      pBC[1][2] = sensorVal[(i)    ][j + 1];  pBC[1][3] = sensorVal[(i)    ][j + 2];
      pBC[2][0] = sensorVal[(i + 1)][j - 1];  pBC[2][1] = sensorVal[(i + 1)][j    ];
      pBC[2][2] = sensorVal[(i + 1)][j + 1];  pBC[2][3] = sensorVal[(i + 1)][j + 2];
      pBC[3][0] = sensorVal[(i + 2)][j - 1];  pBC[3][1] = sensorVal[(i + 2)][j    ];
      pBC[3][2] = sensorVal[(i + 2)][j + 1];  pBC[3][3] = sensorVal[(i + 2)][j + 2];
      bcp.updateCoefficients(pBC);
      recordSampleData(false, ratio, i, j); //false: isBoundary
    }
  }
}

void recordSampleData(bool isBoundary, int ratio, int i, int j) {
  float sampleX, sampleY, sampleM;
  int w, h;
  for (int x = 0; x < ratio; x++) {
    for (int y = 0; y < ratio; y++) {
      sampleX = (float)x / (float)ratio;
      sampleY = (float)y / (float)ratio;
      w = i * ratio + x;
      h = j * ratio + y;
      if (isBoundary) {
        sampleM = blp.getValue(sampleX, sampleY);
      } else {
        sampleM = bcp.getValue(sampleX, sampleY);
      }
      if(sampleM>127) sampleM = 127;
      if(sampleM<-127) sampleM = -127;
      sampleVal[w][h] = sampleM;
    }
  }
}
