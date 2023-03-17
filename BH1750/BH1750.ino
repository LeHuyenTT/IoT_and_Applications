#include <Wire.h>
#include "bh1750.h"
#define ADD_BH1750 0x23
#define DATA_REG_RESET 0x07
#define BH1750_DEFAULT_MTREG 69
#define BH1750_MTREG_MIN 31
#define BH1750_MTREG_MAX 254
#define MODE_H 0x10     //CHM: Continuously H-Resolution Mode
#define MODE_H2 011   //CHM_2: Continuously H-Resolution Mode2
#define MODE_L 0x13     //CLM: Continuously L-Resolution Mode
#define ONE_HR 0x20     //OTH: One Time H-Resolution Mode
#define ONE_HR2 0x21   //OTH_2: One Time H-Resolution Mode2
#define ONE_LR 0x23      //OTL: One Time L-Resolution Mode

float measuringTimeFactor = 1; // MTReg / 69 default
uint8_t mode = MODE_H; //chọn mode default
byte MTreg = BH1750_DEFAULT_MTREG; //chọn measurement time regisiter default 
long lastTime = 0;
void setup(){
  Serial.begin(9600);
  Wire.begin();
  Configure(mode, lastTime); //cấu hình mode
  setMTreg(BH1750_DEFAULT_MTREG, mode); //set measurement time regisiter
}
void loop(){ 
  if(Delay(180, BH1750_DEFAULT_MTREG, lastTime, mode)){
    //cho delay với giá trị max
    float val = readLight(mode, measuringTimeFactor);//đọc Light
    //xuất giá trị
    Serial.print(F("Light is: "));
    Serial.print(val);
    Serial.println(" lux");
    delay(1000);
  }
}
