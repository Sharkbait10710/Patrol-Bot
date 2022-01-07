#ifndef MOTOR_H
#define MOTOR_H

//Define hardware motor pins
#define pwmA 12
#define In1A 13 //"In" pins polarity determines direction of rotation
#define In2A 15
#define pwmB 4
#define In1B 14
#define In2B 2
#define Stby 1

int speed[2] = {0, 0};
void Motor_Setup() {
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(In1A, OUTPUT);
  pinMode(In2A, OUTPUT);
  pinMode(In1B, OUTPUT);
  pinMode(In2B, OUTPUT);
  pinMode(Stby, OUTPUT);
  //Turn off to reduce energy consumption
  digitalWrite(Stby, HIGH);
}

//Run functions take in an int 0-100 to drive the motor from 0 - 100% speed
void run_A(int Speed) {
  if (Speed == speed[0]) return; //Quit function if speed remains unchanged
  Serial.println("THIS SHOULD NOT APPEAR");
  bool Dir = true;
  speed[0] = Speed; //Update speed data
  if (Speed < 0) {Dir = false; Speed = -1*Speed;} //Switch dir if speed is negative
  if (Dir){digitalWrite(In1A, HIGH); digitalWrite(In2A, LOW);}
  else {digitalWrite(In1A, LOW); digitalWrite(In2A, HIGH);}
  analogWrite(pwmA, map(Speed, 0, 100, 0, 255));
}
void run_B(int Speed) {
  if (Speed == speed[1]) return;
  Serial.println("THIS SHOULD NOT APPEAR");
  bool Dir = true;
  speed[1] = Speed;
  if (Speed < 0) {Dir = false; Speed = -1*Speed;}
  if (Dir){digitalWrite(In1B, HIGH); digitalWrite(In2B, LOW);}
  else {digitalWrite(In1B, LOW); digitalWrite(In2B, HIGH);}
  analogWrite(pwmB, map(Speed, 0, 100, 0, 255));
}

void handle_Motor(String spdA, String spdB) {
  run_A(spdA.toInt());
  run_B(spdB.toInt());
}
#endif
