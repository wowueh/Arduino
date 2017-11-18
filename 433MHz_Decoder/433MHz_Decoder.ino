#include <RCSwitch.h>

RCSwitch myReceiver = RCSwitch();

void setup() {
  // put your setup code here, to run once:
myReceiver.enableReceive(0);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
if (myReceiver.available())
{
  
  
  unsigned long i = myReceiver.getReceivedValue();
  int l = myReceiver.getReceivedBitlength();
  Serial.print("RAW: ");
  Serial.print(i,BIN);
  Serial.print("\t");
  Serial.print("length: ");
  Serial.print(l);
  Serial.print("\t");
  Serial.print("addr: ");
  Serial.print(i>>8,BIN);
  Serial.print("\t");
  Serial.print("command: ");
  Serial.println((i<<24)>>24,BIN);
  myReceiver.resetAvailable();
}
//if (myReceiver.available()==0)
//{
//  Serial.println("");
//}
}
