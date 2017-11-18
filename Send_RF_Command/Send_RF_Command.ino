#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();   // Initiate RF Tranmitter Object
int RFpin = 10;   // This pin on Nano is used to send data to RF Tranmitter




void setup() {
  // put your setup code here, to run once:
mySwitch.enableTransmit(RFpin);
delay(1000);
unsigned long address = 0b1111100110010100;
mySwitch.send((address << 8) | 0b11000000, 24);
mySwitch.send((address << 8) | 0b11000000, 24);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
