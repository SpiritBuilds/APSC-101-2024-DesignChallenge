/* 
 * APSC 101 Example Sketch
 * 
 * M.Malik for APSC 101: 22-Sep-2023
 * 
 * Simple example code for testing a DC Motor. Original code from Adafruit.
 * 
 * To understand how to use the library methods, Read documentation on the AFMotor library at: https://learn.adafruit.com/afmotor-library-reference/af-dcmotor
 * 
 */


#include <AFMotor.h>

AF_DCMotor motor(1);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps -- **Ensure Serial Monitor also has baud rate set to 9600**
  Serial.print("Starting motor.");
  motor.setSpeed(255);
  motor.run(FORWARD);
}

void loop() {
}
