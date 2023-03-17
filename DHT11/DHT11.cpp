#include"DHT11.h"
#include <Arduino.h>

void Start_Signal(int PIN){//delay 18ms để chờ tín hiệu Start từ Master gửi cho Slaver
  pinMode(PIN, OUTPUT); //lúc này Pin sẽ là chân Output
  digitalWrite(PIN, LOW); // tín hiệu LOW trong 18ms
  delay(18);
  digitalWrite(PIN, HIGH);  //đặt chân tín hiệu sang High
  pinMode(PIN, INPUT_PULLUP); // lúc này Pin sẽ là chân Input
}
bool Respon(int PIN){//nếu Slaver phản hồi trong khảng 80ns thì việc lấy dữ liệu được sãn sàng
  unsigned long TOFF_TIME = pulseIn(PIN, LOW); //Kiểm tra phản hồi khi tín hiệu LOW
//  Serial.println(TOFF_TIME);
   if (TOFF_TIME <= 88 && TOFF_TIME >= 76)  // Nếu tín hiệu low trong đoạn [76;88] thì được 
                                            //phản hổi, ngược lại thì không
     return 1;
    else return 0;
}

      