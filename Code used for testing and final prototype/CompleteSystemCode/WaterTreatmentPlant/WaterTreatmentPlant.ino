#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>
//A0: final, A2:mixing turbidity sensors
#define Estop 21 //Emergency stop pushbutton connected between pin 21 and ground
volatile bool emergency = false;
uint8_t UserButtonPin = 23; //The pin that the button the user interacts with is asstached to, could also be a value like A2
uint8_t CapacitiveSensor = 52;
bool UserPressed = false;
bool pumpingCentrifugal = false;
bool pumpingPeristaltic = false;
bool mixing = false;
uint8_t safeLight = 43;
uint8_t loadingLight = 44;
uint8_t notSafeLight = 45;
//Ease of life variables
const unsigned long minute = 60000;
const float mixThreshold = 3.01;
const float endThreshold = 3.87;
//Variables necessary for turbidity
uint8_t finalSensor = A0;
uint8_t mixingSensor = A2;
//const unsigned long time = 5000;
long startTime;

bool mixed = false;
bool pumpedToFinal = false;
bool done = false;

AF_DCMotor mixingMotor(1);
AF_DCMotor CentrifugalPump(2);
AF_DCMotor PeristalticPump(4);
Servo Servo1;
//mix for 1:30, settle 4mins

void setup(){
  Serial.begin(9600); //Start the serial monitor, for debugging and user experience
  Serial.println("Power On");
  pinMode(Estop, INPUT_PULLUP); // Setup the arduino to accept input from Estop pin
  attachInterrupt(digitalPinToInterrupt(Estop), Emergency, CHANGE); // makes the Estop pin an interrupt pin
  //when pressed and the button goes to ground, it will call the emergency punction
  pinMode(UserButtonPin, INPUT_PULLUP);//Internal pullup resistor on arduino, preps user input
  pinMode(CapacitiveSensor, INPUT_PULLUP);
  Servo1.attach(9);
  Servo1.write(0);
  //Light setup
  pinMode(safeLight, OUTPUT);
  pinMode(loadingLight, OUTPUT);
  pinMode(notSafeLight, OUTPUT);

  CentrifugalPump.setSpeed(160);
  PeristalticPump.setSpeed(255);
  mixingMotor.setSpeed(200);
  
  startTime = millis();

}

void loop(){

  if(digitalRead(Estop) == HIGH){
    emergency = true;
  }
  if(emergency){
    shutDown();
  }
  if (!done) {
    if (digitalRead(UserButtonPin) == HIGH && !UserPressed){
      UserPressed = true;
      //Start the process: Centrifugual pump
      Serial.println("Start Input Received");
      Serial.println("Commencing processes");
      digitalWrite(loadingLight, HIGH);
      startCFpump();
    }

    //check water level to stop pump, then start pumping
    if (digitalRead(CapacitiveSensor) == LOW && pumpingCentrifugal){
      Serial.println("Coagulation tank full, start mixing");
      stopCFpump();
      dispenseAlum();
      mix(0.5*minute);//1.5minutes
      Serial.println("Solution settling");
      delay(4*minute); //Stop mixing and rest
    }

    if (UserPressed && mixed){
      if(checkWater(mixThreshold, mixingSensor) && !mixing && !pumpedToFinal){
        startPERpump(2*minute);
        pumpedToFinal = true;
      }else if (UserPressed && mixed && !checkWater(mixThreshold, mixingSensor)){
        mix((0.5*minute));
        delay(1.5*minute);
        if(!mixing && !pumpingPeristaltic){//Start Peristalltic pump
          startPERpump(2*minute);
       }
      }
    }
    if(!mixing && !pumpingCentrifugal && !pumpingPeristaltic && pumpedToFinal){
      //checkWater(endThreshold, finalSensor);
      if (!checkWater(endThreshold, finalSensor)) {
        digitalWrite(notSafeLight, HIGH);
        Serial.println("The final water turbidity is not clean enough, consider running the system again");
        done = true;
      } else if (checkWater(endThreshold, finalSensor)) {
        digitalWrite(safeLight, HIGH);
        Serial.println("The water is clean enough to move on!");
        done = true;
      }
    }
  }else{
    Serial.println("The process is complete, check logs for results");
    delay(5000);
  }
  digitalRead(Estop);
  delay(100);
}

void startCFpump(){
  pumpingCentrifugal = true;
  CentrifugalPump.run(FORWARD);
  Serial.println("Centrifugual pump is running");
}
void stopCFpump(){
  pumpingCentrifugal = false;
  CentrifugalPump.run(RELEASE);
  Serial.println("Centrifugal pump stopped");
}
void startPERpump(long duration){
  pumpingPeristaltic = true;
  long startTime = millis();
  Serial.println("Peristaltic pump on");
  while (millis() - startTime < duration){
    PeristalticPump.run(FORWARD);
    if (emergency) {
      shutDown();
    }
  }
  stopPERpump();
}
void stopPERpump(){
  pumpingPeristaltic = false;
  PeristalticPump.run(RELEASE);
  Serial.println("Peristalstic pump stopped");
}
void dispenseAlum(){
  Servo1.write(180);
  delay(1000);
  Servo1.write(0);
} // Could be a start function and a stop function aswell as seen in example

void mix(long duration){
  mixed = false;
  mixing = true;
  long startTime = millis();
  while (millis() - startTime < duration) {
    mixingMotor.run(BACKWARD);
    if (emergency) {
      shutDown();
    }
  }
  stopMixing();
  mixed = true;
}

void stopMixing(){
  mixingMotor.run(RELEASE);
  mixing = false;
}

bool checkWater(float threshold, uint8_t sensor){
  Serial.print("Checking Water ");
  if (threshold == mixThreshold){
    Serial.print("for Mixing");
  } else{
    Serial.print("for final");
  }
  const unsigned long interval = 750;
  unsigned long currentTime = 0;
  unsigned long previousTime = 0;
  uint8_t iteration = 0;
  float voltageSum =0;
  while(iteration < 7){
    currentTime = millis();
    if (currentTime - previousTime >= interval){
      previousTime = currentTime;
      iteration++;
      float endSensor = analogRead(sensor);
      float endVoltage = endSensor * (5.0/1024.0);
      voltageSum += endVoltage;
    }
  }
  float averageVoltage = voltageSum/7.0;
  if(averageVoltage > threshold){
    Serial.println("Water is clean!");
    Serial.print("Voltage is ");
    Serial.println(averageVoltage);
    if(threshold == endThreshold){
      digitalWrite(safeLight, HIGH);
      digitalWrite(notSafeLight, LOW);
      return true;
    }
  }
  else {
    Serial.print("Voltage is ");
    Serial.println(averageVoltage);
    return false;
  }
}

void shutDown(){
  //What to do when there is an emergency
  PeristalticPump.setSpeed(0);
  PeristalticPump.run(RELEASE);
  pumpingPeristaltic = false;

  CentrifugalPump.setSpeed(0);
  CentrifugalPump.run(RELEASE);
  pumpingCentrifugal = false;
  Serial.println("Pumps stopped");

  stopCFpump();
  stopPERpump();
  stopMixing();

  while (1){
    //purposfully gets the arduino stuck in this loop to prevent it from doing anything until reset
    //Set of an LED (blinking?) and print alram state every 5 seconds
    Serial.println("Emergency Stop, reset to continue");
    digitalWrite(safeLight, HIGH);

    digitalWrite(notSafeLight, HIGH);
    digitalWrite(loadingLight, HIGH);
    delay(500);
    digitalWrite(safeLight, LOW);
    digitalWrite(notSafeLight, LOW);
    digitalWrite(loadingLight, LOW);
    delay(100);
  }
}

void Emergency(){
  emergency = true;
  shutDown();// not recommended, rather you should call other functions in the loop based on the changed state
}

