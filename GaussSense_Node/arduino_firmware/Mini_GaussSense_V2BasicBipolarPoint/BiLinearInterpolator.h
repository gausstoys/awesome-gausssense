//
//  BiLinearInterpolator.h
//  cvHelloWorld
//
//  Created by  on 12/8/18.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef cvHelloWorld_BiLinearInterpolator_h
#define cvHelloWorld_BiLinearInterpolator_h

class BiLinearInterpolator
{
    
public:    
	void updateCoefficients (float p[2][2]) {
		a1 = p[0][0];
		a2 = p[1][0]-p[0][0];
		a3 = p[0][1]-p[0][0];
		a4 = p[0][0]-p[1][0]-p[0][1]+p[1][1];
	}
    
	float getValue (float x, float y) {
		return a1+x*a2+y*a3+x*y*a4;
	}
private:
    float a1, a2, a3, a4;
};

#endif
