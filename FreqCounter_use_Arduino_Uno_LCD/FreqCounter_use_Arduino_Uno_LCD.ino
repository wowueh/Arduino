#include <Wire.h>

#include <LiquidCrystal595.h>

//Thu tu tren mach LCD: Green is pin 7; Yellow is pin 8; Purple is pin 9

LiquidCrystal595 lcd(7, 8, 9);

const int pulsePin = 12; // Input signal connected to Pin 12 of Arduino

int pulseHigh; // Integer variable to capture High time of the incoming pulse

int pulseLow; // Integer variable to capture Low time of the incoming pulse

float pulseTotal; // Float variable to capture Total time of the incoming pulse

float frequency; // Calculated Frequency

void setup() {

pinMode(pulsePin, INPUT);

lcd.begin(16, 2);

lcd.setCursor(0, 0);

lcd.print("Instructables");

lcd.setCursor(0, 1);

lcd.print(" Freq Counter ");

delay(5000);

}

void loop() {

lcd.setCursor(0, 0);

lcd.print("Frequency is ");

lcd.setCursor(0, 1);

lcd.print(" ");

pulseHigh = pulseIn(pulsePin, HIGH);

pulseLow = pulseIn(pulsePin, LOW);

pulseTotal = pulseHigh + pulseLow; // Time period of the pulse in microseconds

frequency = 1000000 / pulseTotal; // Frequency in Hertz (Hz)

lcd.setCursor(0, 1);

lcd.print(frequency);

lcd.print(" Hz");

delay(500);

}
