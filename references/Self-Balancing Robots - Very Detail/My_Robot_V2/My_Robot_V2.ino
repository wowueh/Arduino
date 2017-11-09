#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#include <AFMotor.h>
AF_DCMotor motorA(2, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm
AF_DCMotor motorB(4, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm


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
float p = 0;
float i = 0;
float d = 0;
float kP = 50;
float kI = 0;
float kD = 0;
float setAngle = 0;
float lastTime = millis();
float lastPitch = setAngle;
float beginTime = millis();
float angleRange = 5;
float sensitive = 10;

// =========================================================
// ============== MOTOR           ==========================
// =========================================================
boolean motorDirect = 0;
boolean brake = 1;
int adjSpeedMotor = 5;

void setup() {
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
  mpu.setXGyroOffset(-256);
  mpu.setYGyroOffset(65);
  mpu.setZGyroOffset(49);
  mpu.setXAccelOffset(-2548);
  mpu.setYAccelOffset(-350);
  mpu.setZAccelOffset(3703);

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
  if (millis() - beginTime < 10000) {
    goto wait01;
  }
setA:
  setAngle = calPitch();
  if (setAngle == 0) {
    goto setA;
  }
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - beginTime > 10000)
  {
    float pitchV = calPitch();
    //      Serial.print(pitchV);
    //      Serial.print("\t");
    //      Serial.print(calPID(pitchV));
    //      Serial.print("\t");
    //      Serial.println(setAngle);
    MoveControl(calPID(pitchV));
  }
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
  p = kP * error;
  i += kI * error * timeGap;

  d = kD * (pitch - lastPitch) / timeGap;
  int PID = int(p + i + d);

  if (PID > 0) {
    PID = PID + 30;
  }
  if (PID < 0) {
    PID = PID -30
    \
    
    
    ;
  }

  if (PID > 255) {
    PID = 255;
  } else if (PID < -255) {
    PID = -255;
  }

  lastTime = millis();
  lastPitch = pitch;
  return PID;

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
void MoveControl(int PID)
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

