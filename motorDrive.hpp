#pragma once
#include <Servo.h>


Servo rightMotor; // Create a servo object to control a servo motor
Servo leftMotor;

const int leftMotorPin = 14; // Define the pin to which the servo is connected
const int rightMotorPin = 18;


void attachMotors(){
    leftMotor.attach(leftMotorPin); // Attaches the servo on pin 9 to the servo object
    rightMotor.attach(rightMotorPin); // Attaches the servo on pin 9 to the servo object

}

void drive(int rightSpeed, int leftSpeed){
    if(rightSpeed <30 ){
      rightSpeed = 40;
    }
    else if(rightSpeed>85){
      rightSpeed = 85;
    }
  
  
    if(leftSpeed > 150){
      leftSpeed = 160;
    }
    else if(leftSpeed<95){
      leftSpeed = 95;
    }
  
    //RMS=rightSpeed;
    //LMS=leftSpeed;
    rightMotor.write(rightSpeed); // right 
    leftMotor.write(leftSpeed); // left
  
    //delay(3000);
  
  }
  
  void _90DegreeTurn(char direction){
    
    if(direction == 'l'){
      rightMotor.write(65);
      leftMotor.write(90);
      delay(750); // og 1000
      rightMotor.write(90);
      leftMotor.write(90);
    }
    else{
      rightMotor.write(90);
      leftMotor.write(115);
      delay(750); // og 1000
      rightMotor.write(90);
      leftMotor.write(90);
    }
  }
  
void moveForward(){
  rightMotor.write(70); //og 75
  leftMotor.write(105); // og 105
  delay(250);
  rightMotor.write(90);
  leftMotor.write(90);
}
void moveBackward(){
  rightMotor.write(100); // 105
  leftMotor.write(70); // og 75
  delay(250);
  rightMotor.write(90);
  leftMotor.write(90);
}

void turnAround(){
  rightMotor.write(65);
  leftMotor.write(65);
  delay(1250);
  rightMotor.write(90);
  leftMotor.write(90);
}

void motorStop(){
  rightMotor.write(90);
  leftMotor.write(90);
}