#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  //Chân phát là chân số 8.
  mySwitch.enableTransmit(8);
  pinMode(4, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  unsigned long address = 0b0101010101010101;
  /*
    Chúng ta phải để address là unsigned long vì:
    + address sẽ là biến nhận 24 bit dữ liệu má hóa nên kích thước của nó phải đủ lớn để chứa 16 bit địa chỉ và 8 bit dữ liệu
    + nó dương (unsigned) để chúng ta chuyển nó sang thập phân cho dễ (khỏi quân tâm đến số âm)
  */
  /*
    Code blink D3
  */

   if (digitalRead(4) == HIGH) {
    mySwitch.send((address << 8) | 0b00000000, 24);
    digitalWrite(13, HIGH);
    delay(2000);
    digitalWrite(13, LOW);
  }
//  mySwitch.send((address << 8) | 0b00000011, 24);
//  delay(1000);
//  mySwitch.send((address << 8) | 0b00000000, 24);
//  delay(1000);
}
