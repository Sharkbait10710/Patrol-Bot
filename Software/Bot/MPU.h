#ifndef MPU_H
#define MPU_H
#include <Adafruit_MPU6050.h>
#include <BasicLinearAlgebra.h>
#include <ArduinoJson.h>

using namespace BLA;

Adafruit_MPU6050 MPU;
uint8_t MPUaddr = 0x68;
sensors_event_t a, g, temp;
double delta_V[3] = {0, 0, 0}; //Velocity: x | y | z
double delta_S[3] = {0, 0, 0}; //Position
double delta_O[3] = {0, 0, 0}; //Ang Vel 
double delta_R[3] = {0, 0, 0}; //Rotation
StaticJsonDocument<7> MPU_Data; //Change size depending on how much data you want to send
char MPU_Json[100]; 

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
  delta_O[0] = delta_O[0] + g.gyro.x*Time;
  delta_O[1] = delta_O[1] + g.gyro.y*Time;
  delta_O[2] = delta_O[2] + g.gyro.z*Time;
  for (int i = 0; i < 3; i++) 
    delta_R[i] = delta_R[i] + delta_O[i]*Time;
    
  BLA::Matrix<3, 3> x_rot;
  x_rot(1,1)=1                           ; x_rot(1,2)=0                           ; x_rot(1,3)=0                           ;
  x_rot(2,1)=0                           ; x_rot(2,2)=cos(delta_R[0]*71.0/4068.0) ; x_rot(2,3)=-sin(delta_R[0]*71.0/4068.0);
  x_rot(3,1)=0                           ; x_rot(3,2)=sin(delta_R[0]*71.0/4068.0) ; x_rot(3,3)=cos(delta_R[0]*71.0/4068.0) ;
  BLA::Matrix<3, 3> y_rot;
  y_rot(1,1)=cos(delta_R[1]*71.0/4068.0) ; y_rot(1,2)=0                           ; y_rot(1,3)=sin(delta_R[1]*71.0/4068.0) ;
  y_rot(2,1)=0                           ; y_rot(2,2)=1                           ; y_rot(2,3)=0                           ;
  y_rot(3,1)=-sin(delta_R[1]*71.0/4068.0); y_rot(3,2)=0                           ; y_rot(3,3)=cos(delta_R[0]*71.0/4068.0) ;
  BLA::Matrix<3, 3> z_rot;
  z_rot(1,1)=cos(delta_R[2]*71.0/4068.0) ; z_rot(1,2)=-sin(delta_R[2]*71.0/4068.0); z_rot(1,3)=0                           ;
  z_rot(2,1)=sin(delta_R[2]*71.0/4068.0) ; z_rot(2,2)=cos(delta_R[2]*71.0/4068.0) ; z_rot(2,3)=0                           ;
  z_rot(3,1)=0                           ; z_rot(3,2)=0                           ; z_rot(3,3)=1                           ;
  BLA::Matrix<3> IMU_accel; 
  IMU_accel(1) = a.acceleration.x        ; IMU_accel(2) = a.acceleration.y        ; IMU_accel(3) = a.acceleration.z        ;
  BLA::Matrix<3> ref_Acc = x_rot*y_rot*z_rot*IMU_accel;

  for (int i = 0; i < 3; i++)
    delta_V[i] = delta_V[i] + ref_Acc(i+1)*Time;
  for (int i = 0; i < 3; i++) 
    delta_S[i] = delta_S[i] + delta_V[i]*Time;
}

void update_Pos_Rot() {
  poll();
  MPU_Data["delta_S.x"] = delta_S[0];
  MPU_Data["delta_S.y"] = delta_S[1];
  MPU_Data["delta_S.z"] = delta_S[2];
  MPU_Data["delta_R.x"] = delta_R[0];
  MPU_Data["delta_R.y"] = delta_R[1];
  MPU_Data["delta_R.z"] = delta_R[2];
  MPU_Data["temp"     ] = temp.temperature;
  serializeJson(MPU_Data, MPU_Json);
}
#endif
