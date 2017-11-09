#include <AFMotor.h>
 
AF_DCMotor motor(2, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm 
AF_DCMotor motorb(4, MOTOR12_64KHZ); // tạo động cơ #2, 64KHz pwm 
 
void setup() {  
    
    Serial.begin(9600);           // mở cổng Serial monitor 9600 bps 
    
    Serial.println("Hello my racing");     
    
    motor.setSpeed(30);     // chọn tốc độ maximum 255`/255
    motorb.setSpeed(35);

    Serial.print("tien");     
    
    motor.run(FORWARD);      // động cơ tiến 
    motorb.run(FORWARD);
    
    delay(1000);    
    
    Serial.print("lui");  
    
    motor.run(BACKWARD);     // động cơ lùi  
    motorb.run(BACKWARD);
    
    delay(1000);     
    
    Serial.print("tack");  
    
    motor.run(RELEASE);      //   dừng động cơ
    motorb.run(RELEASE);
    
    delay(1000);
 
}  
 
void loop() {  
 
    
}
