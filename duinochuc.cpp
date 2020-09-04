
#include <duinochuc.h>
#include <Wire.h>
//-- DEFINES

// Address of the wii chuck is 0x52
#define ADDRESS 0x52


// Controller initialization.  Apparently, this is 0x40 0x00 for some controllers.
#define INIT_B1 0xF0
#define INIT_B2 0x55

// Some chucks obfuscate the returned data, I am told. Uncomment to enable decryption.
// #define WIICRYPTION

void DuinoChuc::init() {
    Wire.beginTransmission(ADDRESS);
    Wire.write(INIT_B1);
    Wire.write(INIT_B2);
    Wire.endTransmission();
}

void DuinoChuc::readData() {
    int i=0;
    uint8_t buf[6];
    
    Wire.beginTransmission(ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(10);
    Wire.requestFrom(ADDRESS,6);
    
    while(Wire.available() && i<6) {
        buf[i++] = Wire.read();
    }
    this->b_z = buf[5] & 0x01;
    this->b_c = (buf[5] & 0x02) >>1;
    this->accelX = (int16_t)( 
        (uint16_t)buf[2]<<2 | (((uint16_t)buf[5] >> 2) & 0x03)
        )-512;
    this->accelY = (int16_t)(
        (uint16_t)buf[3]<<2 | (((uint16_t)buf[5] >> 4) & 0x03)
        )-512;
    this->accelZ = (int16_t)(
        (uint16_t)buf[4]<<2 | (((uint16_t)buf[5] >> 6) & 0x03)
        )-512;
    this->joyX = buf[0];
    this->joyY = buf[1];
    
}

int16_t DuinoChuc::getJoyX() {
    return this->joyX;
}
int16_t DuinoChuc::getJoyY() {
    return this->joyY;
}
int16_t DuinoChuc::getAccelX() {
    return this->accelX;
}
int16_t DuinoChuc::getAccelY() {
    return this->accelY;
}
int16_t DuinoChuc::getAccelZ() {
    return this->accelZ;
}
bool DuinoChuc::getButtonC() {
    return this->b_c;
}
bool DuinoChuc::getButtonZ() {
    return this->b_z;
}


DuinoChuc chuck;

