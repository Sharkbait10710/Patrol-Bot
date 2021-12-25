#ifndef SERVO_H
#define SERVO_H
#include <ESP32Servo.h>

#define servoPin 3
Servo servo;
int servo_pos = 50;

void servo_Setup() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);    // standard 50 hz servo
  servo.attach(servoPin, 500, 2400); 
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
  servo.write(map(servo_pos, 0, 100, 0, 180));
}

void rotate_servo(int R) {
  servo.write(map(R, 0, 100, 0, 180));
}

#endif
