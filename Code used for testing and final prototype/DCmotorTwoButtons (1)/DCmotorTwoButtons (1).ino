/*
 * APSC 101 Example Sketch
 * 
 * Adapted from the Adafruit Motor Sheild Library example "MotorTest" 
 * Modified by R.Smyk 2024.08.09
 * 
 * This code takes input from two push buttons to turn a DC motor quickly 
 * or slowly. 
 * 
 */

// Include the motor shield V1 library
#include <AFMotor.h>

// Define the pins the push buttons and limit switch are connected to
#define pushButtonSlow A2
#define pushButtonFast A3

// Connect the DC motor to M1 on the motor control board
AF_DCMotor DCmotor(1);

// Define variables to store the states of the push buttons
bool pushButtonStateSlow = HIGH;
bool pushButtonStateFast = HIGH;

void setup() {
  // Set up serial communication to print things to the serial monitors
  Serial.begin (9600);

  // Set the speed of the DC motor. The speed input can be 0-255.
  DCmotor.setSpeed(255);

  // Start the motor off not moving
  DCmotor.run(RELEASE);

  // Configure the pins to to be input pins
  pinMode(pushButtonSlow, INPUT_PULLUP);
  pinMode(pushButtonFast, INPUT_PULLUP);
}

void loop() {
  // Read the value of the pin as either HIGH or LOW
  pushButtonStateSlow=digitalRead(pushButtonSlow);
  pushButtonStateFast=digitalRead(pushButtonFast);

  // Turn the motor slowly or quickly depending on which button is pressed
  if (pushButtonStateSlow==LOW){
    Serial.println("Slow");
    DCmotor.run(FORWARD);
    DCmotor.setSpeed(75);
  }
  else if (pushButtonStateFast==LOW){
    Serial.println("Fast");
    DCmotor.run(FORWARD);
    DCmotor.setSpeed(255);
  }


  // Wait 50 milliseconds before starting the void loop over again
  delay(50);
}
  
