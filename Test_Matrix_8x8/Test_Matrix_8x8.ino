/*
      8x8 LED Matrix MAX7219 Example 01
   by Dejan Nedelkovski, www.HowToMechatronics.com
   Based on the following library:
   GitHub | riyas-org/max7219  https://github.com/riyas-org/max7219
*/
#include <MaxMatrix.h>
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

int h = 0;
int t = 0;
long last= millis();
bool dot = true;

int DIN = 9;   // DIN pin of MAX7219 module
int CLK = 8;   // CLK pin of MAX7219 module
int CS = 7;    // CS pin of MAX7219 module
int maxInUse = 1;
MaxMatrix m(DIN, CS, CLK, maxInUse);


/* ++++++++++++ DEFINE CHARACTERS ++++++++++++*/
char A[] = {4, 8,
            B01111110,
            B00010001,
            B00010001,
            B01111110,
           };
char B[] = {4, 8,
            B01111111,
            B01001001,
            B01001001,
            B00110110,
           };
char U[] = {4, 8,
            B00011111,
            B00100000,
            B00100000,
            B00011111,
           };
char p[] = {4, 8,
            B11111100,
            B00100100,
            B00011000,
            B00000000,
           };
char d[] = {4, 8,
            B00000000,
            B00011000,
            B00100100,
            B00111111,
           };
char a[] = {4, 8,
            B00011000,
            B00100100,
            B00010100,
            B00111100,
           };
char tt[] = {4, 8,
            B00000100,
            B00111110,
            B00100100,
            B00010000,
           };
char i[] = {4, 8,
            B00100100,
            B00111100,
            B00100100,
            B00000000,
           };
char n[] = {4, 8,
            B00111100,
            B00001000,
            B00000100,
            B00111100,
           };
char g[] = {4, 8,
            B00011000,
            B00100100,
            B10010100,
            B11111100,
           };
char smile01[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10010101,
                  B10100001,
                  B10100001,
                  B10010101,
                  B01000010,
                  B00111100
                 };
char smile02[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10010101,
                  B10010001,
                  B10010001,
                  B10010101,
                  B01000010,
                  B00111100
                 };
char smile03[] = {8, 8,
                  B00111100,
                  B01000010,
                  B10100101,
                  B10010001,
                  B10010001,
                  B10100101,
                  B01000010,
                  B00111100
                 };

char one[] = {3, 6,
              B100010,
              B111111,
              B100000,
             };
char two[] = {3, 6,
              B110010,
              B101001,
              B100110,
             };
char three[] = {3, 6,
                B100101,
                B100101,
                B011010,
               };
char four[] = {3, 6,
               B001100,
               B001010,
               B111111,
              };
char five[] = {3, 6,
               B100111,
               B100101,
               B011001,
              };
char six[] = {3, 6,
              B111110,
              B101001,
              B111000,
             };
char seven[] = {3, 6,
                B000001,
                B111001,
                B000111,
               };
char eight[] = {3, 6,
                B011010,
                B100101,
                B011010,
               };
char nine[] = {3, 6,
               B000111,
               B100101,
               B011111,
              };
char zero[] = {3, 6,
               B011110,
               B100001,
               B011110,
              };
char square8[] = {8, 8,
                  B11111111,
                  B10000001,
                  B10000001,
                  B10000001,
                  B10000001,
                  B10000001,
                  B10000001,
                  B11111111,
                 };
char square6[] = {6, 6,
                  B111111,
                  B100001,
                  B100001,
                  B100001,
                  B100001,
                  B111111,
                 };
char square4[] = {4, 4,
                  B1111,
                  B1001,
                  B1001,
                  B1111,
                 };
char square2[] = {2, 2,
                  B11,
                  B11,
                 };
                 
char fillSquare8[] = {8, 8,
                  B11111111,
                  B11111111,
                  B11111111,
                  B11111111,
                  B11111111,
                  B11111111,
                  B11111111,
                  B11111111,
                 };
char fillSquare6[] = {6, 6,
                  B111111,
                  B111111,
                  B111111,
                  B111111,
                  B111111,
                  B111111,
                 };
char fillSquare4[] = {4, 4,
                  B1111,
                  B1111,
                  B1111,
                  B1111,
                 };

char degreeC[] = {9, 6,
                  B000000,
                  B000000,
                  B000011,
                  B000011,
                  B000000,
                  B011110,
                  B111111,
                  B110011,
                  B010010,
                 };
char percent[] = {8, 6,
                  B000000,
                  B000000,
                  B100000,
                  B010011,
                  B001011,
                  B110100,
                  B110010,
                  B000001,
                 };
/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/


void printMsg()
{
  int io =8;
  int s = 5;
  m.clear();
  m.writeSprite(io, 0, U);
  io+=s;
  m.writeSprite(io, 0, p);
  io+=s;
  m.writeSprite(io, 0, d);
  io+=s;
  m.writeSprite(io, 0, a);
  io+=s;
  m.writeSprite(io, 0, tt);
  io+=s;
  m.writeSprite(io, 0, i);
  io+=s;
  m.writeSprite(io, 0, n);
  io+=s;
  m.writeSprite(io, 0, g);
  for (int j = 0; j < io+5; j++) {
    m.shiftLeft(false, false);
    delay(100);
  }
  m.clear();
}

//This method is used to display the number is less than 100:
void displayNumber(unsigned int num, int startX, int startY)
{
  if (num < 100)  //Check if the number is less than 100
  {
    int numOfDigit = String(num).length(); //Count number of digits
    String numStr = String(num);

    for (int i = 0; i < numOfDigit; i++) //Display each digit
    {
      int x = (i * 4) + startX;
      int y = startY;
      int digit = numStr.substring(i, i + 1).toInt();

      switch (digit)
      {
        case 0:
          m.writeSprite(x, y, zero);
          break;
        case 1:
          m.writeSprite(x, y, one);
          break;
        case 2:
          m.writeSprite(x, y, two);
          break;
        case 3:
          m.writeSprite(x, y, three);
          break;
        case 4:
          m.writeSprite(x, y, four);
          break;
        case 5:
          m.writeSprite(x, y, five);
          break;
        case 6:
          m.writeSprite(x, y, six);
          break;
        case 7:
          m.writeSprite(x, y, seven);
          break;
        case 8:
          m.writeSprite(x, y, eight);
          break;
        case 9:
          m.writeSprite(x, y, nine);
          break;
        default:
          // if nothing else matches, do the default
          // default is optional
          break;
      }
    }
  }
}

/*This method is used to scroll 2 number (each number is less than 100)
  You can set space between them in pixel, speed of scroll and how long
  the number is still by using params:   space,   spd,    tm          */
void scrollTwoNum(int num1, int num2, int space, int spd, int tm)
{
//  m.setDot(7, 0, true);
//  m.setDot(space + 13, 0, true);
//  m.setDot(space + 20, 0, true);
  displayNumber(num1, 8, 1);
  displayNumber(num2, space + 14, 1);
  for (int i = 0; i < 7; i++) {
    m.shiftLeft(false, false);
    delay(spd);
  }
  delay(tm);
  for (int i = 0; i < space + 6; i++) {
    m.shiftLeft(false, false);
    delay(spd);
  }
  delay(tm);
}

void scrollHandT(int num1, int num2,int spd)
{
  displayNumber(num1, 8, 1);
  m.writeSprite(15, 1, percent);
  displayNumber(num2, 32, 1);
  m.writeSprite(39, 1, degreeC);
  
  for (int i = 0; i < 48; i++) {
    m.shiftLeft(false, false);
    delay(spd);
  }
}

//This method zoom-in then zoom-out the Square Box
void zoomSquareBox(int spd)
{
  //noInterrupts();
  //Zoom-out
  m.clear();
  delay(spd);
  m.writeSprite(0, 0, square8);
  delay(spd);
  m.clear();
  m.writeSprite(1, 1, square6);
  delay(spd);
  m.clear();
  m.writeSprite(2, 2, square4);
  delay(spd);
  m.clear();
  m.writeSprite(3,3, square2);
  delay(spd);
  //Zoom-out
  m.clear();
  delay(spd);
  m.writeSprite(3,3, square2);
  delay(spd);
  m.clear();
  m.writeSprite(2, 2, square4);
  delay(spd);
  m.clear();
  m.writeSprite(1, 1, square6);
  delay(spd);
  m.clear();
  m.writeSprite(0, 0, square8);
  delay(spd);
  m.clear();


//interrupts();
}

//This method zoom-in then zoom-out the Square Box
void zoomFillSquareBox(int spd)
{
  //noInterrupts();

  //Zoom-out
  m.clear();
  delay(spd);
  m.writeSprite(0, 0, fillSquare8);
  delay(spd);
  m.clear();
  m.writeSprite(1, 1, fillSquare6);
  delay(spd);
  m.clear();
  m.writeSprite(2, 2, fillSquare4);
  delay(spd);
  m.clear();
  m.writeSprite(3,3, square2);
  delay(spd);

  //Zoom-in
  m.clear();
  delay(spd);
  m.writeSprite(3,3, square2);
  delay(spd);
  m.clear();
  m.writeSprite(2, 2, fillSquare4);
  delay(spd);
  m.clear();
  m.writeSprite(1, 1, fillSquare6);
  delay(spd);
  m.clear();
  m.writeSprite(0, 0, fillSquare8);
  delay(spd);
  m.clear();
//interrupts();
}



void setup() {

  //Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2 pinMode(led,OUTPUT);
  m.init(); // MAX7219 initialization
  m.setIntensity(1
  ); // initial led matrix intensity, 0-15
//    printMsg();
//    zoomSquareBox(100);
//    zoomFillSquareBox(100);
//    m.writeSprite(0, 0, smile01);
//    delay(500);
//    m.clear();
//    m.writeSprite(0, 0, smile02);
//    delay(500);
//    m.clear();
//    m.writeSprite(0, 0, smile03);
//    delay(500);
//    m.clear();


}
void loop() {
  if (mySwitch.available())
  {
    if ((int)mySwitch.getReceivedValue() > 40) {
      h = (int)mySwitch.getReceivedValue();
      //Serial.println(h);
    } else
    {
      t = (int)mySwitch.getReceivedValue();
      //Serial.println(t);
    }
    mySwitch.resetAvailable();
  }

//  if (h != 0 && t != 0)
//  {
    scrollTwoNum(h, t, 20, 30, 1000);
    m.clear();
    scrollHandT(h,t,100);
    m.clear();
//  } else
//  {
      
//    if(millis()-last>1000)
//    {
//      int x = (int)random(0,8);
//      int y = (int)random(0,8);
//      m.setDot(x, y, dot);
//      dot=!dot;
//      last=millis();
//    }
//

}

