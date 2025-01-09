 /*
 * APSC 101 Example Sketch
 * 
 * C. Milberry for APSC 101, 2023.08.9
 * 
 * Simple example code for servo motor with Arduino. 
 * More info: https://www.instructables.com/Arduino-How-to-Control-Servo-Motor-With-Motor-Shie/
 * 
 */


#include <AFMotor.h>
#include <Servo.h>
Servo Servo1;

int servo_pos = 0;

void setup() {
  // put your setup code here, to run once:
  Servo1.attach(9);
  Servo1.write(0);

}

void loop() {
  Servo1.write(180);
  delay(1000);
  Servo1.write(0);
  delay(10000);
}
