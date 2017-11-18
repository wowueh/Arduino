/* rawSend.ino Example sketch for IRLib2
 *  Illustrates how to send a code Using raw timings which were captured
 *  from the "rawRecv.ino" sample sketch.  Load that sketch and
 *  capture the values. They will print in the serial monitor. Then you
 *  cut and paste that output into the appropriate section below.
 */
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <EEPROM.h>

IRsendRaw mySender;

#define RAW_DATA_LEN 60
uint16_t rawData[RAW_DATA_LEN];

void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  Serial.println(F("Type the EEPROM addr of command to send. (Please refer to 'EEPROM Code Map')"));
  
}

void loop() {
    if(Serial.available() >0)
    {
    int i = Serial.read() - '0';
    //send a code every time a character is received from the 
    // serial port. You could modify this sketch to send when you
    // push a button connected to an digital input pin.
    EEPROM.get(i*120, rawData);
    mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    Serial.println(F("Sent signal."));
    }
}

