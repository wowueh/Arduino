#include <SoftwareSerial.h>
#include <RCSwitch.h>

RCSwitch myReceiver = RCSwitch();
SoftwareSerial bluetooth(9, 10);

void setup() {
  // put your setup code here, to run once:
myReceiver.enableReceive(0);
bluetooth.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
if (myReceiver.available())
{
  unsigned long i = myReceiver.getReceivedValue();
  int l = myReceiver.getReceivedBitlength();
  bluetooth.print("RAW: ");
  bluetooth.print(i,BIN);
  bluetooth.print("\t");
  bluetooth.print("length: ");
  bluetooth.print(l);
  bluetooth.print("\t");
  bluetooth.print("addr: ");
  bluetooth.print(i>>8,BIN);
  bluetooth.print("\t");
  bluetooth.print("command: ");
  bluetooth.println((i<<24)>>24,BIN);
  myReceiver.resetAvailable();
}
//if (myReceiver.available()==0)
//{
//  bluetooth.println("");
//}
}
