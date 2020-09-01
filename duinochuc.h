
#include <Arduino.h>

#ifndef DUINOCHUC_H
#define DUINOCHUC_H


class DuinoChuc {
public:
    void init();
    void readData();
    
    int16_t getJoyX();
    int16_t getJoyY();
    int16_t getAccelX();
    int16_t getAccelY();
    int16_t getAccelZ();
    bool getButtonC();
    bool getButtonZ();
    
private:
    int16_t joyX, joyY;
    int16_t accelX, accelY, accelZ;
    bool b_z, b_c;
};

extern DuinoChuc chuck;

#endif
