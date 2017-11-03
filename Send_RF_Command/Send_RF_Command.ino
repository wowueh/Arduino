#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();   // Initiate RF Tranmitter Object
int RFpin = 4;   // This pin on Nano is used to send data to RF Tranmitter




void setup() {
  // put your setup code here, to run once:
mySwitch.enableTransmit(RFpin);
delay(1000);
unsigned long address = 0b0101010101010111;
mySwitch.send((address << 8) | 0b00000000, 24);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
