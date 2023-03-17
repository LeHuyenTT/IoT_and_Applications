#define DHT11_PIN 32  // chọn chân kết nối 32 trên ESP32
#include "DHT11.h"
unsigned long TON_TIME = 0;   // Thời gian tín hiệu ở mức cao
unsigned long TOFF_TIME = 0;  // Thời gian tín hiệu ở mức thấp

unsigned char data_byte[5];    // Khai báo một mảng 5 byte tương tứng mới 5 byte trong DATA
unsigned int data_packet[40];  // Khai báo một mảng 40 phần tử tương ứng với 40 bit DATA
unsigned char bit_data = 0;
unsigned char checksum_byte = 0;  //Kiểm tra việc truyền bit đúng hay không ?
int bit_counter = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Humidity & Temperature Sensor\n");
  delay(1000);  //Wait before accessing Sensor
}

void loop() {
  Start_Signal(DHT11_PIN);       //ESP32 gửi tín hiệu Start cho DHT11
  if (Respon(DHT11_PIN) == 1) {  
    //Nếu xác nhận đã Respon thì bắt đầu nhận tín hiệu dữ liệu

    while (1) {
      TON_TIME = pulseIn(DHT11_PIN, HIGH);     
      //Tính thời gian tín hiệu ở mức cao
      if (TON_TIME <= 28 && TON_TIME >= 20) {  
        //nếu thời gian đó nằm trong đoạn [20;28] 
        //thì coi như data truyền đến là mức 0
        bit_data = 0;
      } else if (TON_TIME <= 74 && TON_TIME >= 65) {  
        //nếu thời gian đó nằm trong đoạn [65;74] 
        //thì coi như data truyền đến là mức 1
        bit_data = 1;
      } else if (bit_counter == 40) {  
        //Khi đã đủ 40 bit dữ liệu thì thoát vòng lặp
        break;
      }
      data_byte[bit_counter / 8] |= bit_data << (7 - (bit_counter % 8)); 
       //Lưu data vào từng byte dữ liệu (có tất cả 5 byte - mỗi byte 8 bit) =>
      //lần lượt đưa các bit vào byte tương ứng của nó bằng cách dịch trái vào vị trí tương ứng
        // ví dụ counter = 0 => lưu vào data_byte [0] => bit_data sẽ dịch 7 bit
    data_packet[bit_counter] = bit_data; // lưu các bit vào mảng 40 bit
    bit_counter++;    // tăng giá trị counter 1 đơn vị
    }
  }
  checksum_byte = data_byte[0] + data_byte[1] + data_byte[2] + data_byte[3]; 
   //checksum bằng cách cộng 4 byte đầu với nhau

  if (checksum_byte == data_byte[4] && checksum_byte != 0) {  
    //Kiểm tra điều kiện của check_sum =>đúng thì xuất giá trị
    Serial.println("Humidity: ");
    for (int c = 0; c <= 7; c++) {  //in từng bit độ ẩm
      Serial.print(data_packet[c]);
    }
    //Serial.print("\t");
    Serial.print("  ");
    for (int c = 0; c <= 7; c++) {
      Serial.print(data_packet[c + 8]);
    }
    Serial.print("\t");
    Serial.print(data_byte[0]);  //in giá trị độ ẩm
    Serial.print(".");
    Serial.print(data_byte[1]);


    Serial.print("%");
    Serial.println(" ");

    Serial.println("Temperature: ");
    for (int c = 0; c <= 7; c++) {  //in từng bit nhiệt độ
      Serial.print(data_packet[c + 16]);
    }
    Serial.print("  ");
    for (int c = 0; c <= 7; c++) {
      Serial.print(data_packet[c + 24]);
    }
    Serial.print("\t");
    Serial.print(data_byte[2]);  //in giá trị nhiệt độ
    Serial.print(".");
    Serial.print(data_byte[3]);

    Serial.print("C");
    Serial.println(" ");

    Serial.println("Checksum Byte: ");
    for (int c = 0; c <= 7; c++) {  //in từng bit checksum
      Serial.print(data_packet[c + 32]);
    }
    Serial.println(" ");
    Serial.print("CHECKSUM_OK");
    Serial.println("");
    Serial.println("");
  }
  //reset giá trị ban đầu để tiếp tục đo giá trị mới
  bit_counter = 0;
  data_byte[0] = data_byte[1] = data_byte[2] = data_byte[3] = data_byte[4] = 0;
  delay(1000);
}