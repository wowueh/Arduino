#include <EEPROM.h>

#define RAW_DATA_LEN 60
uint16_t rawData[RAW_DATA_LEN]={
  8418, 4242, 506, 1602, 506, 554, 526, 526, 
  530, 526, 530, 1578, 502, 554, 526, 526, 
  530, 526, 530, 526, 526, 526, 530, 526, 
  530, 522, 530, 526, 530, 526, 530, 526, 
  526, 526, 530, 1578, 502, 1606, 506, 1602, 
  506, 550, 530, 526, 530, 1578, 502, 554, 
  526, 530, 526, 526, 530, 526, 530, 1578, 
  502, 554, 526, 1000};

  
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial.println(sizeof(rawData));
EEPROM.put(0, rawData);
}

void loop() {
  // put your main code here, to run repeatedly:

}
