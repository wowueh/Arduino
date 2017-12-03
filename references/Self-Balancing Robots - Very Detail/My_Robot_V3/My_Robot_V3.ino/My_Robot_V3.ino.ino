/********* SELF-BALANCING 2 WHEEL ROBOT ********************************************************************
AUTHOR: ANDYHD88 email: wowueh@gmail.com

This is a 2 wheels Self Balancing Robot which is able to
recieve movement and setting commands through Bluetooth.

* HARDWARE:
  - 01 Arduino Uno R3
  - 01 Adafruit Motor Shield L298D
  - 01 MPU6050 GY-521 6DOF Breakout board.
  - 01 HC-05 Bluetooth Module
  - 02 5V motors
  - 01 7.4V battery
  - 02 Heatsink for 2 H-Brigde on Shield (Optional)
  - 01 Switch for Power Supply (Optional)
  - Acrylic Chasis

* SOFTWARE:
  - Bluetooth Controller installed on Smartphone (You can go to https://ai2.appinventor.com to create one)
  - Upload this below sketch to your Arduino

* WIRING:  
  A. MOTORS AND SHIELD:
    - Connect 02 motors to M2,M4 Port on Shield (Red wire is outside).
    - Connect Battery to Shield (Use jumper to supply power to both Shield and Arduino).
  
  B. MPU6050 AND SHIELD:
      INT ------> D2
      VCC ------> 5V
      GND ------> GND
      SDA ------> A4
      SCL ------> A5
  
  C. HC-05 AND SHIELD:
      VCC ------> 5V
      GND ------> GND
      TX  ------> D9
      RX  ------> D10
  
*************************************************************************************************************/
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#include <AFMotor.h>
AF_DCMotor motorA(2, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm
AF_DCMotor motorB(4, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm


#include <SoftwareSerial.h>
#define TX_PIN 10
#define RX_PIN 9
SoftwareSerial bluetooth(RX_PIN, TX_PIN);

int baudRate[] = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
byte command = 4;

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

#define OUTPUT_READABLE_YAWPITCHROLL

// Unccomment if you are using an Arduino-Style Board
#define ARDUINO_BOARD

#define LED_PIN 13      // (Galileo/Arduino is 13)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion q;           // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

// This function is not required when using the Galileo
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

// ================================================================
// ===              PID Controller                          =======
// ================================================================
int PID = 0;
float p = 0;
float i = 0;
int addKi = 1; //USE IN INTERGRAL CALCULATION, DEFAULT = 1
float d = 0; 
float kP = 9.5;
float kI = 0.1;
float kD = 500;
float setAngle = 0; //BALANCE POINT
float tiltAngle1 = 0;
float tiltAngle2 = 0;
float saveAngle = 0;
float lastTime = millis();
float lastPitch = setAngle;
float beginTime = millis();
//float angleRange = 5;
float sensitive = 10; // USE TO SET MINIMUM ANGLE CHANGE TO RESPOND
float dP = 0; // USE TO ADD TO PID TO MAKE ROBOT MOVE, DEFAULT = 0


// =========================================================
// ============== MOTOR           ==========================
// =========================================================
boolean motorDirect = 0;
boolean brake = 1;
int adjSpeedMotor = 10; 
int minMotorSpeed = 30;

void setup() {

  bluetooth.begin(115200);


  // put your setup code here, to run once:
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //int TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);
  while (!Serial);

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(F("MPU6050 connection "));
  Serial.print(mpu.testConnection() ? F("successful") : F("failed"));

  // wait for ready
  //    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
  //    while (Serial.available() && Serial.read()); // empty buffer
  //    while (!Serial.available());                 // wait for data
  //    while (Serial.available() && Serial.read()); // empty buffer again

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(-252);
  mpu.setYGyroOffset(69);
  mpu.setZGyroOffset(52);
  mpu.setXAccelOffset(-1570);
  mpu.setYAccelOffset(-365);
  mpu.setZAccelOffset(2719);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
wait01:
  if (millis() - beginTime < 14000) {
    goto wait01;
  }
  digitalWrite(LED_PIN, HIGH);
wait02:
  if (millis() - beginTime < 20000) {
    goto wait02;
  }
  digitalWrite(LED_PIN, LOW);
setA:
  setAngle = calPitch();
  if (calPitch()) {
    setAngle = calPitch();
    saveAngle = setAngle;
    tiltAngle1 = setAngle - 1;
    tiltAngle2 = setAngle + 1;
  } else {
    goto setA;
  }
  digitalWrite(LED_PIN, HIGH);
  //  Serial.println(setAngle);
  //  Serial.println(saveAngle);
  //  Serial.println(tiltAngle1);
  //  Serial.println(tiltAngle2);
  //

}











void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - beginTime > 10000)
  {
    float pitchV = calPitch();
    //          Serial.print(pitchV);
    //          Serial.print("\t");
    //          Serial.print(calPID(pitchV));
    //          Serial.print("\t");
    //          Serial.println(setAngle);
    PID = calPID(pitchV);
    KeepBalance(PID);
  }

  // Recieve command from Smartphone
  if (bluetooth.available()) {
    command = bluetooth.read();
    if (command == 'F')
    {
      //    motorA.setSpeed(minMotorSpeed+20);
      //    motorA.run(FORWARD);
      //    motorB.setSpeed(minMotorSpeed+20+20);
      //    motorB.run(FORWARD);
      //  setAngle=tiltAngle1;
      dP = 80;
    } else if (command == 'B')
    {
      //    motorA.setSpeed(minMotorSpeed+20);
      //    motorA.run(BACKWARD);
      //    motorB.setSpeed(minMotorSpeed+20+20);
      //    motorB.run(BACKWARD);

      //  setAngle= tiltAngle2;
      dP = -80;
    } else if (command == 'S')
    {
      //    setAngle = saveAngle;
      //    i = 0;
      dP = 0;
    }
    else if (command == 'P')
    {
      kP = bluetooth.parseFloat();
    }
    else if (command == 'I')
    {
      kI = bluetooth.parseFloat();
    }
    else if (command == 'D')
    {
      kD = bluetooth.parseFloat();
    }
    else if (command == '-')
    {
      setAngle -= 0.1;
    }
    else if (command == '+')
    {
      setAngle += 0.1;
    }
    else if (!isAlpha(command)&&isAlphaNumeric(command))
    {
      addKi = int(command);
    }

    // CLEAR BUFFER TO AVOID ERROR
    for (int i = 1;i <=64; i++)
    {
      bluetooth.read();
    }
  }


  // Send param status to Smartphone
//  bluetooth.print("PID:");
//  bluetooth.print(PID);
//  bluetooth.print("\t");
//  bluetooth.print("P:");
//  bluetooth.print(p);
//  bluetooth.print("\t");
//  bluetooth.print("I:");
//  bluetooth.print(i);
//  bluetooth.print("\t");
//  bluetooth.print("D:");
//  bluetooth.println(d);
//
//  bluetooth.print("kP:");
//  bluetooth.print(kP);
//  bluetooth.print("\t");
//  bluetooth.print("kI:");
//  bluetooth.print(kI);
//  bluetooth.print("\t");
//  bluetooth.print("kD:");
//  bluetooth.print(kD);
}













float calPitch()
{
  float pitch = 0;
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available

#ifdef ARDUINO_BOARD
  while (!mpuInterrupt && fifoCount < packetSize) {
  }
#endif

#ifdef GALILEO_BOARD
  delay(10);
#endif

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;


#ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //Serial.print("ypr\t");
    //            Serial.print("Pitch\t");
    //            Serial.print(ypr[0] * 180/M_PI);
    //            Serial.print("\t");
    //            Serial.println(ypr[1] * 180/M_PI);
    //            Serial.print("\t");
    //            Serial.println(ypr[2] * 180/M_PI);
#endif

    // blink LED to indicate activity
    //    blinkState = !blinkState;
    //    digitalWrite(LED_PIN, blinkState);
  }
  pitch = ypr[1] * 180 / M_PI;
  return pitch;
}


// =========================================================
// ============== PID CALCULATION ==========================
// =========================================================
int calPID(float pitch)
{
  float timeGap = millis() - lastTime;
  //  pitch = int(pitch);
  float error = round((pitch - setAngle) * sensitive) / sensitive;
  p = kP * error + dP;
  if (abs(i) < 5) {
    i += kI * error * timeGap;
    //  }else if(abs(i)>=5 && abs(i)<30){
    //  i += kI * error * timeGap * 4;
  } else {
    i = constrain(i + kI * error * timeGap * addKi, -255, 255);
  }
  d = constrain(kD * (pitch - lastPitch) / timeGap,-255,255);
  int PID = int(p + i + d);

  //  Serial.print("P= ");
  //  Serial.print(p);
  //  Serial.print("\t");
  //  Serial.print("I= ");
  //  Serial.print(i);
  //  Serial.print("\t");
  //  Serial.print("D= ");
  //  Serial.println(d);


  if (PID > 0) {
    PID = map(abs(PID), 0, 255, minMotorSpeed, 255);
  }
  if (PID < 0) {
    PID = -map(abs(PID), 0, 255, minMotorSpeed, 255);
  }

  if (PID > 255) {
    PID = 255;
  } else if (PID < -255) {
    PID = -255;
  }

  lastTime = millis();
  lastPitch = pitch;
  return PID;

  bluetooth.print(PID);

}

// =========================================================
// ============== MOTOR CONTROL   ==========================
// =========================================================

void Move(boolean direction, int PID)
{
  int adjspeed = PID + adjSpeedMotor;
  int speed = PID;

  if (adjspeed > 255)
  {
    adjspeed = 255;
  } else if (adjspeed < -255)
  {
    adjspeed = -255;
  }
  motorA.setSpeed(speed);
  motorB.setSpeed(adjspeed);

  //  if (brake == 0 && motorDirect != direction)
  //  {
  //    motorA.run(RELEASE);
  //    motorB.run(RELEASE);
  //    brake = 1;
  //  }

  brake == 0;
  if (direction == 0)
  {
    motorA.run(FORWARD);
    motorB.run(FORWARD);
    motorDirect = 0;
  }
  else if (direction == 1)
  {
    motorA.run(BACKWARD);
    motorB.run(BACKWARD);
    motorDirect = 1;
  }
}


// ============================================================
// ============== MOVEMENT CONTROL   ==========================
// ============================================================
void KeepBalance(int PID)
{
  if (PID > 0)
  {
    Move(0, PID);
  }
  else if (PID < 0)
  {
    Move(1, -PID);
  }
  else if (PID == 0)
  {
    //    motorA.run(RELEASE);
    //    motorB.run(RELEASE);
    motorA.setSpeed(255);
    motorB.setSpeed(255);
    brake = 1;
    if (motorDirect == 0)
    {
      motorA.run(BACKWARD);
      motorA.run(RELEASE);
      motorB.run(BACKWARD);
      motorB.run(RELEASE);
    } else if (motorDirect == 1) {
      motorA.run(FORWARD);
      motorA.run(RELEASE);
      motorB.run(FORWARD);
      motorB.run(RELEASE);
    }


  }
}

