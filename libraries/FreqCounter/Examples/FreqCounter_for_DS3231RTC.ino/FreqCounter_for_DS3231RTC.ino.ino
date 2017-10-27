// Frequency Counter Lib example

/*
  Martin Nawrath KHM LAB3
  Kunsthochschule f¸r Medien Kˆln
  Academy of Media Arts
  http://www.khm.de
  http://interface.khm.de/index.php/labor/experimente/  
 */
#include <FreqCounter.h>
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <TimeLib.h>         //http://playground.arduino.cc/Code/Time
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire


unsigned long frq;
int cnt;
int pinLed=13;

void setup() {
  pinMode(pinLed, OUTPUT);

  Serial.begin(115200);        // connect to the serial port

  RTC.squareWave(SQWAVE_8192_HZ);    //Set frequency for DS3231RTC
  
  Serial.println("Frequency Counter");

}



void loop() {

  // wait if any serial is going on
  FreqCounter::f_comp=10;   // Cal Value / Calibrate with professional Freq Counter
  FreqCounter::start(1000);  // 1000 ms Gate Time / Count numbers of interupt in 1000 ms

  while (FreqCounter::f_ready == 0) 

  frq=FreqCounter::f_freq;
  Serial.print(cnt++);
  Serial.print("  Freq: ");
  Serial.println(frq);
  delay(20);
  //digitalWrite(pinLed,!digitalRead(pinLed));  // blink Led

}  

