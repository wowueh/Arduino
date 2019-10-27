#include <RCSwitch.h>

RCSwitch myReceiver = RCSwitch();

void setup() {
  // put your setup code here, to run once:
myReceiver.enableReceive(0);
Serial.begin(9600);
pinMode(4,OUTPUT);
digitalWrite(4, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
if (myReceiver.available())
{
  unsigned long i = myReceiver.getReceivedValue();
  Serial.println(i);
  if(i==16515110)
  {
    digitalWrite(4, HIGH);
    Serial.println("ALARM  !!!!!!!!!!!!!!!!!!!!!");
    delay(30000);
  }
  else
  {
  digitalWrite(4, LOW);
  }
  myReceiver.resetAvailable();
}
}
