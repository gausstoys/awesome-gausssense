class GData{
public:
    float x, y, intensity, angle, pitch;
    
    GData(int _x, int _y, int _intensity){
        set((float)_x, (float)_y, (float)_intensity, -1, -1);
    }

    GData(float _x, float _y, float _intensity){
        set(_x, _y, _intensity);
    }

    GData(int _x, int _y, int _intensity, int _angle, int _pitch){
        set((float)_x, (float)_y, (float)_intensity, (float)_angle, (float)_pitch);
    }

    GData(float _x, float _y, float _intensity, float _angle, float _pitch){
        set(_x, _y, _intensity, _angle, _pitch);
    }

    void set(float _x, float _y, float _intensity){
        set(_x, _y, _intensity, 0, 0);
    }

    void set(float _x, float _y, float _intensity, float _angle, float _pitch){
        x = _x;
        y = _y;
        intensity = _intensity;
        angle = _angle;
        pitch = _pitch;
    }

    int getPolarity() {
        if(intensity < 0) {
            return 1;
        }
        return 0;
    }
    
    float getX() {
        return x;
    }
    
    float getY() {
        return y;
    }
    
    float getIntensity() {
        return (intensity < 0 ? -intensity : intensity);
    }
    
    float getAngle() {
        return angle;
    }
    
    float getPitch() {
        return pitch;
    }
    
};
