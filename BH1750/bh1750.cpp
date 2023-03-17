#include "bh1750.h"

#define MODE_H 0x10     //CHM: Continuously H-Resolution Mode
#define MODE_H2 011   //CHM_2: Continuously H-Resolution Mode2
#define MODE_L 0x13     //CLM: Continuously L-Resolution Mode
#define ONE_HR 0x20     //OTH: One Time H-Resolution Mode
#define ONE_HR2 0x21   //OTH_2: One Time H-Resolution Mode2
#define ONE_LR 0x23      //OTL: One Time L-Resolution Mode
#define ADD_BH1750 0x23
#define BH1750_DEFAULT_MTREG 69  // measurement time regisiter default
#define BH1750_MTREG_MIN 31// measurement time regisiter min
#define BH1750_MTREG_MAX 254 // measurement time regisiter max

bool Configure(uint8_t mode, long lastTime)
{
  byte ack = 5; //chưa có gì xãy ra
  lastTime = millis();
  switch (mode){
    case MODE_H:     //CHM: Continuously H-Resolution Mode
    case MODE_H2:  //CHM_2: Continuously H-Resolution Mode2
    case MODE_L:    //CLM: Continuously L-Resolution Mode
    case ONE_HR:     //OTH: One Time H-Resolution Mode
    case ONE_HR2:   //OTH_2: One Time H-Resolution Mode2
    case ONE_LR:     //OTL: One Time L-Resolution Mode

      Wire.beginTransmission(ADD_BH1750); //Bắt đầu truyền tải các yêu cầu đến cảm biến
      Wire.write((uint8_t(mode)));//Cho phép đo
      Wire.endTransmission();  //Kết thúc việc truyền
      delay(10);
      break;
  }
switch(ack){
   case 0:
    //lastReadTimestamp = millis();
    return true;
  case 1: // too long for transmit buffer
    Serial.println(F("[BH1750] ERROR: too long for transmit buffer"));
    break;
  case 2: // received NACK on transmit of address
    Serial.println(F("[BH1750] ERROR: received NACK on transmit of address"));
    break;
  case 3: // received NACK on transmit of data
    Serial.println(F("[BH1750] ERROR: received NACK on transmit of data"));
    break;
  case 4: // other error
    Serial.println(F("[BH1750] ERROR: other error"));
    break;
  default:
    Serial.println(F("[BH1750] ERROR: undefined error"));
    break;
  }

  return false;
} 
bool Delay(bool Max_w, byte MTreg, long lastTime, byte mode)
{
  long delay = 0;
  switch(mode){
    case MODE_H:     //CHM: Continuously H-Resolution Mode
    case MODE_H2:  //CHM_2: Continuously H-Resolution Mode2   
    case ONE_HR:     //OTH: One Time H-Resolution Mode
    case ONE_HR2:   //OTH_2: One Time H-Resolution Mode2
     { Max_w ?  delay =(180*MTreg/69):(120*MTreg/69);
     //nếu Max delay thì  delay =(180*MTreg/69), ngược lại delay =(120*MTreg/69)
      break;} 
    case MODE_L:    //CLM: Continuously L-Resolution Mode
    case ONE_LR: 
      { Max_w ?  delay =(24*MTreg/69):(16*MTreg/69); //tương tự
      break; }
  }
  long currentTime = millis(); //Delay tính từ lúc Configure đến lúc đọc xong dữ liệu
  if(currentTime - lastTime >= delay){
    return true;}
  else return false;
  
}
bool setMTreg(byte MTreg, byte mode){ //cấu hình cho measurement time regisiter
  if (MTreg < BH1750_MTREG_MIN || MTreg > BH1750_MTREG_MAX) {
    Serial.println(F("[BH1750] ERROR: MTreg out of range"));
    return false;
  }
byte ack = 5;
// Send MTreg and the current mode to the sensor
  //Tăng độ phân giải
  Wire.beginTransmission(ADD_BH1750);  
  Wire.write((0b01000 << 3) | (MTreg >> 5));//   High bit: 01000_MT[7,6,5]
  ack = Wire.endTransmission();
  Wire.beginTransmission(ADD_BH1750);
  Wire.write((0b011 << 5) | (MTreg >> 0b11111));//    Low bit: 011_MT[4,3,2,1,0]
  ack |= Wire.endTransmission();
  Wire.beginTransmission(ADD_BH1750);
  Wire.write(mode); //Truyền mode
  ack |= Wire.endTransmission(); //kết thúc việc truyền

  delay(10);
    switch (ack) { //xét ACK
  case 0:
    return true;
  case 1: // too long for transmit buffer
    Serial.println(F("[BH1750] ERROR: too long for transmit buffer"));
    break;
  case 2: // received NACK on transmit of address
    Serial.println(F("[BH1750] ERROR: received NACK on transmit of address"));
    break;
  case 3: // received NACK on transmit of data
    Serial.println(F("[BH1750] ERROR: received NACK on transmit of data"));
    break;
  case 4: // other error
    Serial.println(F("[BH1750] ERROR: other error"));
    break;
  default:
    Serial.println(F("[BH1750] ERROR: undefined error"));
    break;
  }

  return false;
}

uint16_t readData_BH1750(){ //Đọc dữ liệu thô và lưu trong 2 byte High và Low
                            // sau đó trả về dạng 16bit
  uint8_t HIGH_byte, LOW_byte;
  Wire.requestFrom(ADD_BH1750, 2);
  if(Wire.available()){
    HIGH_byte=Wire.read();
    LOW_byte=Wire.read(); 
  }
  return ((HIGH_byte<<8) | LOW_byte);
}
float readLight(uint8_t mode, float measuringTimeFactor){ //đọc giá trị Light
  uint16_t rawLux;
  float lux;
  rawLux = readData_BH1750();//gọi hàm readData_BH1750
  if((mode == 0b00010001) || (mode == 0b00100001)){ // mode MODE_H2 || mode ONE_HR2
    lux = (rawLux/2.4)/measuringTimeFactor;     
  }
  else{ //CÁC MODE HIGH
    lux = (rawLux/1.2)/measuringTimeFactor;
  }
  return lux;

}
