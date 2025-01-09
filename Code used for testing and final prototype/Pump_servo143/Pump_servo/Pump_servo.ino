#include <AFMotor.h> 

 

 

AF_DCMotor Port1(1); 

AF_DCMotor Port2(2); 

AF_DCMotor Port3(3); 

AF_DCMotor Port4(4); 

 

//Servo servo; 

 

void setup(){ 

  Serial.begin(9600); 

  Serial.println("Hello World"); 

  //servo.attach(9); 

 

  Port1.setSpeed(200); 

  //Port2.setSpeed(100); 

  //Port3.setSpeed(255); 

  //Port4.setSpeed(255); 

  Port1.run(BACKWARD); 

  //Port2.run(FORWARD); 

  //Port3.run(FORWARD); 

  //Port4.run(FORWARD); 

} 

 

void loop(){ 

  // servo.write(90); 

  // delay(2000); 

  // servo.write(0); 

  // delay(500); 
  
  //Port3.run(FORWARD);
  Port1.run(BACKWARD);
} 

 