/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how to use ESP8266 Shield (with AT commands)
  to connect your project to Blynk.

  WARNING!
    It's very tricky to get it working. Please read this article:
    http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ffc2800bf47846f2b101effcfd3f0751";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "vinaphone";
char pass[] = "nhatkim@412";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

// Comment this out to disable: 
// Server to send the most recent values for all widgets. In other words, all analog/digital pin states will be restored 
// and every virtual pin will generate BLYNK_WRITE event.
//BLYNK_CONNECTED() {
//    Blynk.syncAll();
//}


      /* +++++++++++++++++++++++++ CODE FOR DHTxx SENSOR +++++++++++++++++++++++++++++++*/
          #include <DHT.h>
          #include <SPI.h>
          
          #define DHTPIN 6          // What digital pin we're connected to
          
          // Uncomment whatever type you're using!
          #define DHTTYPE DHT11     // DHT 11
          //#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
          //#define DHTTYPE DHT21   // DHT 21, AM2301
          
          DHT dht(DHTPIN, DHTTYPE);
          BlynkTimer timer;
          
          // This function sends Arduino's up time every second to Virtual Pin (5).
          // In the app, Widget's reading frequency should be set to PUSH. This means
          // that you define how often to send data to Blynk App.
          void sendSensor()
          {
            float h = dht.readHumidity();
            float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
          
            if (isnan(h) || isnan(t)) {
              Serial.println("Failed to read from DHT sensor!");
              return;
            }
            // You can send any value at any time.
            // Please don't send more that 10 values per second.
            Blynk.virtualWrite(V5, h);
            Blynk.virtualWrite(V6, t);
          }
      /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/



        /*++++++++++++++++++++++++ CODE FOR 433MHZ RF TRANMITTER +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
          #include <RCSwitch.h>
          RCSwitch mySwitch = RCSwitch();   // Initiate RF Tranmitter Object
          int RFpin = 4;   // This pin on Nano is used to send data to RF Tranmitter
      
          /* --- DEVICE #01  --- OGRANGE BULB
            --------------------------------------------------------------------------------------------------------------------------*/
                  BLYNK_WRITE(V0)  // This Virtual pin is assigned to Orange Bulb - Bedroom
                  {
                    unsigned long address = 0b0101010101010111;   // address is used to identify a specific device in RF tranmission.
                    int value = param.asInt();    // Get value as integer
                  
                    // Check if the button is pressed:
                    if (value == 1) {
                      mySwitch.send((address << 8) | 0b00000000, 24); // Send TURN-ON command
                    }
                    else if (value == 0){
                      mySwitch.send((address << 8) | 0b00000001, 24); // Send TURN-OFF command
                    }
                  }
          /* --- DEVICE #02  --- WHITE NEON TUBE
            --------------------------------------------------------------------------------------------------------------------------*/
                  BLYNK_WRITE(V1)  // This Virtual pin is assigned to NEON TUBE - Bedroom
                  {
                    unsigned long address = 0b0101010101010110;   // address is used to identify a specific device in RF tranmission.
                    int value = param.asInt();    // Get value as integer
                  
                    // Check if the button is pressed:
                    if (value == 1) {
                      mySwitch.send((address << 8) | 0b00000000, 24); // Send TURN-ON command
                    }
                    else if (value == 0) {
                      mySwitch.send((address << 8) | 0b00000001, 24); // Send TURN-ON command
                    }
                  }
           /* --- DEVICE #03  --- WALL FAN
            ----------------------------------------------------------------------------------------------------------------------------*/
                  BLYNK_WRITE(V2)  // This Virtual pin is assigned to WALL FAN - Bedroom
                  {
                    unsigned long address = 0b0101010101010100;   // address is used to identify a specific device in RF tranmission.
                    int value = param.asInt();    // Get value as integer
                  
                    // Check if the button is pressed:
                    if (value == 1) {
                      mySwitch.send((address << 8) | 0b00000000, 24); // Send TURN-ON command
                    }
                    else if (value == 0) {
                      mySwitch.send((address << 8) | 0b00000001, 24); // Send TURN-ON command
                    }
                  }
             /* ---- SEND TO UNO -----
             -----------------------------------------------------------------------------------*/
                      void sendToRX433()
                      {
                        int h = (int)dht.readHumidity();
                        int t = (int)dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

                        mySwitch.send(h,24);
                        delay(100);
                        mySwitch.send(t,24);
                      }

             //------------------------------------------------------------------------------------
        /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/



//        /*++++++++++++++++++++++++ CODE FOR LCD 1602 MODULE (3 PINS ONLY) +++++++++++++++++++++++++++++++++++++++++++++++++++*/
//            #include <LiquidCrystal595.h>    // include the library
//            
//            LiquidCrystal595 lcd(7,8,9);     // datapin, latchpin, clockpin
//          
//            void updateLCD()
//            {
//              String humid = String(dht.readHumidity());
//              String temp = String(dht.readTemperature());
//              
//              lcd.clear();
//              lcd.setCursor(0,0);

//              lcd.print("                ");
//              lcd.setCursor(0,0);
//              lcd.print(humid);
//              lcd.setCursor(0,1);
//              lcd.print("                ");
//              lcd.setCursor(0,1);
//              lcd.print(temp);                            
//            }
//        /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/



        /*++++++++++++++++++++++++ CODE FOR PIR SENSOR +++++++++++++++++++++++++++++*/
            int PIRpin = 5;  // Digital pin on arduino for PIR
            int TriggerMode = 0;

            // This function write recent PIRpin's value to V7 virtual pin.
            void updatePIRresult ()
            {
              // Please don't send more that 10 values per second.
            int PIRstatus = digitalRead(PIRpin);
            Blynk.virtualWrite(V7,PIRstatus);
            
                // Check whether trigger or not
              if (TriggerMode == 1 && PIRstatus == 1)
              {
                Blynk.virtualWrite(V11,1);
              }else
              {
                Blynk.virtualWrite(V11,0
                );
              }
            }

            BLYNK_WRITE(V10)
            {
              TriggerMode = param.asInt();
            }
        /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx*/


      







void setup()
{
//       /*++++++++++++++++++++++++ CODE FOR LCD 1602 MODULE (3 PINS ONLY) ++++++++++++++++++++++++++++++*/
//          lcd.begin(16,2);             // 16 characters, 2 rows
//          lcd.clear();
//          lcd.setCursor(0,0);
//          lcd.print("Welcome to");      
//          lcd.setCursor(0,1);
//          lcd.print("    CENTRAL UNIT");
//            
//      /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/


  
  // Debug console
  Serial.begin(9600);
  delay(10);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
       

      /*|++++++++++++++++++++++++++ CODE FOR DHTxx SENSOR +++++++++++++++++++++++++++++++++++++|*/
          dht.begin();
          // Setup a function to be called every 5 second
          timer.setInterval(5000L, sendSensor);
      /*|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|*/
    
    
      /*++++++++++++++++++++++++ CODE FOR 433MHZ RF TRANMITTER +++++++++++++++++++++++++++++*/
          mySwitch.enableTransmit(RFpin);
      /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx*/


      /*++++++++++++++++++++++++ CODE FOR PIR SENSOR +++++++++++++++++++++++++++++*/
          pinMode(PIRpin, INPUT);
          // Setup a function to be called every 1 second
          timer.setInterval(1000L, updatePIRresult);
      /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/


//      /*++++++++++++++++++++++++ CODE FOR LCD1602 +++++++++++++++++++++++++++++*/
//          // Setup a function to be called every 1 second
          timer.setInterval(5000L, sendToRX433);
//      /*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/




}













void loop()
{
  Blynk.run();
  
      /*|++++++++++++++++++++++++++ CODE FOR DHTxx SENSOR +++++++++++++++++++++++++++++++++++++|*/
          timer.run();
      /*|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|*/
}

