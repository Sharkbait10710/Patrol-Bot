#ifndef MPU_H
#define MPU_H
//Dependencies
#include <Adafruit_MPU6050.h>
#include <BasicLinearAlgebra.h>
#include <ArduinoJson.h>

//Must transform measurement values with respect to orientation
//to correspond with initial coordinate system
using namespace BLA;

//Create MPU object
Adafruit_MPU6050 MPU;
//I2C address on MPU
uint8_t MPUaddr = 0x68;

//Data types to store MPU measurments
sensors_event_t a, g, temp;
double delta_V[3] = {0, 0, 0}; //Velocity: x | y | z
double delta_S[3] = {0, 0, 0}; //Position
double delta_w[3] = {0, 0, 0}; //Ang Vel 
double delta_R[3] = {0, 0, 0}; //Rotation
//JSON object for storing data; change size to accomodate any needed data
StaticJsonDocument<500> MPU_Data; 
//Stringify JSON for setting over websockets
char MPU_Json[500]; 

//Initialization MPU function
void MPU_setup(TwoWire T) {
  MPU_Data["type"] = "sensor";
  MPU_Data["Name"] = "MPU-6050";
//  if (!MPU.begin(MPUaddr, &T)) {} //Confirm Connection
  MPU.setAccelerometerRange(MPU6050_RANGE_8_G);
  MPU.setGyroRange(MPU6050_RANGE_500_DEG);
  MPU.setFilterBandwidth(MPU6050_BAND_21_HZ);
  MPU.getEvent(&a, &g, &temp);
}

//Poll MPU instruments and stringifies data
void poll() {
  unsigned long Ti = micros(); //micros returns unsigned long type
  MPU.getEvent(&a, &g, &temp); //Obtain measurements
  float Time = (micros() - Ti) / 1000000; //Calculate elapsed time in s

  //Calculate the change in angular velocity O in the x, y, and z axes in deg/s
  delta_w[0] = delta_w[0] + g.gyro.x*Time;
  delta_w[1] = delta_w[1] + g.gyro.y*Time;
  delta_w[2] = delta_w[2] + g.gyro.z*Time;
  //Calculate the change in rotation position R in the x, y, and z axes in deg
  for (int i = 0; i < 3; i++) 
    delta_R[i] = delta_R[i] + delta_w[i]*Time;

  //Must run rotation matrices with respect to x, y, and z axes 
  //Arduino trig functions take in numbers in rad; must convert gyro angles from deg to rad
  BLA::Matrix<3, 3> x_rot; //Rotation matrix about the x axis
  x_rot(1,1)=1                           ; x_rot(1,2)=0                           ; x_rot(1,3)=0                           ;
  x_rot(2,1)=0                           ; x_rot(2,2)=cos(delta_R[0]*71.0/4068.0) ; x_rot(2,3)=-sin(delta_R[0]*71.0/4068.0);
  x_rot(3,1)=0                           ; x_rot(3,2)=sin(delta_R[0]*71.0/4068.0) ; x_rot(3,3)=cos(delta_R[0]*71.0/4068.0) ;
  BLA::Matrix<3, 3> y_rot; //Rotation matrix about the y axis
  y_rot(1,1)=cos(delta_R[1]*71.0/4068.0) ; y_rot(1,2)=0                           ; y_rot(1,3)=sin(delta_R[1]*71.0/4068.0) ;
  y_rot(2,1)=0                           ; y_rot(2,2)=1                           ; y_rot(2,3)=0                           ;
  y_rot(3,1)=-sin(delta_R[1]*71.0/4068.0); y_rot(3,2)=0                           ; y_rot(3,3)=cos(delta_R[0]*71.0/4068.0) ;
  BLA::Matrix<3, 3> z_rot; //Rotation matrix about the z axis
  z_rot(1,1)=cos(delta_R[2]*71.0/4068.0) ; z_rot(1,2)=-sin(delta_R[2]*71.0/4068.0); z_rot(1,3)=0                           ;
  z_rot(2,1)=sin(delta_R[2]*71.0/4068.0) ; z_rot(2,2)=cos(delta_R[2]*71.0/4068.0) ; z_rot(2,3)=0                           ;
  z_rot(3,1)=0                           ; z_rot(3,2)=0                           ; z_rot(3,3)=1                           ;
  BLA::Matrix<3> IMU_accel; //Multiplies with all rotation matrix, in any order, to determine acceleration vector relative to initial xyz axes
  IMU_accel(1) = a.acceleration.x        ; IMU_accel(2) = a.acceleration.y        ; IMU_accel(3) = a.acceleration.z        ;
  BLA::Matrix<3> ref_Acc = x_rot*y_rot*z_rot*IMU_accel; //Acceleration vector relative to initial coordinate system
  //Update the velocities and positions
  for (int i = 0; i < 3; i++)
    delta_V[i] = delta_V[i] + ref_Acc(i+1)*Time;
  for (int i = 0; i < 3; i++) 
    delta_S[i] = delta_S[i] + delta_V[i]*Time;
}

//Polls measurments from MPUand stringifies them
void update_Pos_Rot() {
  poll();
  MPU_Data["delta_S.x"] = delta_S[0];
  MPU_Data["delta_S.y"] = delta_S[1];
  MPU_Data["delta_S.z"] = delta_S[2];
  MPU_Data["delta_R.x"] = delta_R[0];
  MPU_Data["delta_R.y"] = delta_R[1];
  MPU_Data["delta_R.z"] = delta_R[2];
  MPU_Data["temp"] = temp.temperature;
  serializeJsonPretty(MPU_Data, MPU_Json);
}
#endif
