#ifndef BH1750_H
#define BH1750_H

#include "Arduino.h"
#include <wire.h>
bool Configure(uint8_t mode, long lastTime);//Cấu hình
bool setMTreg(byte MTreg, byte mode); //Set measurement time regisiter
uint16_t readData_BH1750();//đọc dữ liệu thô
float readLight(uint8_t mode, float measuringTimeFactor);// đọc Light
bool Delay(bool Max_w, byte MTreg, long lastTime, byte mode); //hàm delay
#endif