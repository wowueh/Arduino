    #include <IRLibAll.h>
     
    IRsend mySender;
     
    void setup() {
      Serial.begin(9600);
    }
     
    void loop() {
//      if (Serial.read() != -1) {
        //send a code every time a character is received from the serial port
        //Sony DVD power A8BCA
        mySender.send(NEC,0x807F42BD, 32);
        delay(10000);
//      }
    }
