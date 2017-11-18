/* rawSend.ino Example sketch for IRLib2
 *  Illustrates how to send a code Using raw timings which were captured
 *  from the "rawRecv.ino" sample sketch.  Load that sketch and
 *  capture the values. They will print in the serial monitor. Then you
 *  cut and paste that output into the appropriate section below.
 */
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <EEPROM.h>
#include <RCSwitch.h>

RCSwitch myReceiver = RCSwitch();

IRsendRaw mySender;

#define RAW_DATA_LEN 60
uint16_t rawData[RAW_DATA_LEN];
unsigned long address = 0b1100000000000000;

void setup() {
  myReceiver.enableReceive(0);
  Serial.begin(9600);
  
}

void loop() {

  if (myReceiver.available())
  {
    unsigned long i = myReceiver.getReceivedValue();
    if(i>>8==address)
    {
      int c = (i<<24)>>24;
      EEPROM.get(c*120, rawData);
      mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
      Serial.println(c);
    }
    myReceiver.resetAvailable();
  }
}

