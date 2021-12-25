#ifndef MPU_H
#define MPU_H
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 MPU;
uint8_t MPUaddr = 0x68;
sensors_event_t a, g, temp;
float delta_V[3] = {0, 0, 0}; //Velocity: x | y | z
float delta_S[3] = {0, 0, 0}; //Position
float delta_O[3] = {0, 0, 0}; //Ang Vel 
float delta_R[3] = {0, 0, 0}; //Rotation
String IMU_DATA = "";

void MPU_setup(TwoWire T) {
  if (!MPU.begin(MPUaddr, &T)) {} //Confirm Connection
  MPU.setAccelerometerRange(MPU6050_RANGE_8_G);
  MPU.setGyroRange(MPU6050_RANGE_500_DEG);
  MPU.setFilterBandwidth(MPU6050_BAND_21_HZ);
  MPU.getEvent(&a, &g, &temp);
}

void poll() {
  uint32_t Ti = micros();
  MPU.getEvent(&a, &g, &temp);
  float Time = (micros() - Ti) / 1000000;
  delta_V[0] = delta_V[0] + a.acceleration.x*Time;
  delta_V[1] = delta_V[1] + a.acceleration.y*Time;
  delta_V[2] = delta_V[2] + a.acceleration.z*Time;
  for (int i = 0; i < 3; i++) 
    delta_S[i] = delta_S[i] + delta_V[i]*Time;
  delta_O[0] = delta_O[0] + g.gyro.x*Time;
  delta_O[1] = delta_O[1] + g.gyro.y*Time;
  delta_O[2] = delta_O[2] + g.gyro.z*Time;
  for (int i = 0; i < 3; i++) 
    delta_R[i] = delta_R[i] + delta_O[i]*Time;
}

int get_Temp() {return temp.temperature;}

void update_Pos_Rot() {
  poll();
  String ret = "";
  for (int i = 0; i < 3; i++) 
    ret += String(delta_S[i]) + "|" + String(delta_R[i]) + "|";
  IMU_DATA = ret;
}
#endif
