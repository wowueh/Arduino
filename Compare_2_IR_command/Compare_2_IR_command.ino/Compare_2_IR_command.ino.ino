/* rawR&cv.ino Example sketch for IRLib2
    Illustrate how to capture raw timing values for an unknow protocol.
    You will capture a signal using this sketch. It will output data the
    serial monitor that you can cut and paste into the "rawSend.ino"
    sketch.
*/
// Recommend only use IRLibRecvPCI or IRLibRecvLoop for best results
#include <IRLibRecvPCI.h>

IRrecvPCI myReceiver(2);//pin number for the receiver
boolean flagC = 0;
int commandLen = 60;
uint16_t command1[60];
uint16_t command2[60];
String diff[60];
int e = 100;

void setup() {
  Serial.begin(9600);
  //delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals TO COMPARE"));
  Serial.print(F("(Each command length must be "));
  Serial.print(commandLen);
  Serial.println(" )");
  Serial.println("");
  Serial.println("Please press command 1 !");
}

void loop() {
  //Continue looping until you get a complete signal received
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength == commandLen)
    {

      if (flagC == 0)
      {
        Serial.print(F("\t"));
        for (bufIndex_t i = 0; i < (commandLen - 1); i++) {
          int m = (int)i;
          command1[i] = recvGlobal.recvBuffer[i + 1];
          Serial.print(recvGlobal.recvBuffer[i + 1], DEC);
          Serial.print(F(", "));
          if ( ((i + 1) % 8) == 0) Serial.print(F("\n\t"));
        }
        command1[commandLen - 1] = 1000;
        Serial.println(F("1000};"));
        //Serial.println("debug");
        myReceiver.enableIRIn();      //Restart receiver
        flagC = 1;
        Serial.println("Please press command 2 !");
      }
      else if (flagC == 1)
      {
        Serial.print(F("\t"));
        for (bufIndex_t i = 0; i < (commandLen - 1); i++) {
          command2[i] = recvGlobal.recvBuffer[i + 1];
          Serial.print(recvGlobal.recvBuffer[i + 1], DEC);
          Serial.print(F(", "));
          if ( ((i + 1) % 8) == 0) Serial.print(F("\n\t"));
        }
        command2[commandLen - 1] = 1000;
        Serial.println(F("1000};"));
        myReceiver.enableIRIn();      //Restart receiver
        flagC = 0;

        //Compare two command:
        int d = 0;
        for (int i = 0; i < commandLen; i++)
        {
          int j = command1[i] - command2[i];
          //Write to diff array
          if (abs(j) > e) diff[i] = String(abs(j));
          if (abs(j) <= e) diff[i] = "---";
          if (abs(j) > d)
          {
            d = abs(j);
          }
        }
        if (d > e)
        {
          Serial.print(F("\t"));
          for (int i = 0; i < commandLen; i++) {
            Serial.print(diff[i]);
            Serial.print(F(", "));
            if ( ((i + 1) % 8) == 0) Serial.print(F("\n\t"));
          }
          Serial.println("");

          Serial.println("");
          Serial.print("DIFFERENT!\t");
          Serial.print("d=");
          Serial.println(d);
          Serial.println("");
        } else {
          Serial.println("");
          Serial.print("THE SAME!\t");
          Serial.print("d=");
          Serial.println(d);
          Serial.println("");
        }

        Serial.println(F("Ready to receive IR signals TO COMPARE"));
        Serial.print(F("(Each command length must be "));
        Serial.print(commandLen);
        Serial.println(" )");
        Serial.println("");
        Serial.println("Please press command 1 !");
      }
    }
    else
    {
      myReceiver.enableIRIn();      //Restart receiver
      Serial.println("Command length is not match! Please try another one!");
    }
  }
}

