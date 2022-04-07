#ifndef PCF_H
#define PCF_H
//Dependencies
#include "PCF8574.h"
#include <ArduinoJson.h>

//Identify all hardware pins
#define PCF_S1_Trig P0 
#define PCF_S1_Echo P1
#define PCF_S2_Trig P2
#define PCF_S2_Echo P3
#define PCF_S3_Trig P6
#define PCF_S3_Echo P7
#define PCF_LED_1 P4
#define PCF_LED_2 P5

//Redefine SCL/SDA pins
uint8_t I2C_SCL = 16;
uint8_t I2C_SDA = 0;
uint8_t PCFaddr = 0x20;

//I2C
TwoWire I2C = TwoWire(0);
PCF8574 pcf8574(&I2C, PCFaddr, I2C_SDA, I2C_SCL);

//Array for holding sonar distance data
float Sonar[3] = {0, 0, 0};
//JSON object for storing data; change size to accomodate any needed data
StaticJsonDocument<500> Sonar_Data; 
//Stringify JSON for setting over websockets
char Sonar_Json[200]; 

void flash_LED_1(int T) {
  pcf8574.digitalWrite(PCF_LED_1, HIGH);
  delay(T);
  pcf8574.digitalWrite(PCF_LED_1, LOW);
  delay(T);
}

void flash_LED_2(int T) {
  pcf8574.digitalWrite(PCF_LED_2, HIGH);
  delay(T);
  pcf8574.digitalWrite(PCF_LED_2, LOW);
  delay(T);
}

//Onboard LED
void onboard_Red_LED(int T) {
  digitalWrite(33, LOW);
  delay(T);
  digitalWrite(33, HIGH);
  delay(T);
}

//Emit propogating sound waves for measuring distance (cm) using HC-SR04
void update_S1() { //Distance in cm
  
  pcf8574.digitalWrite(PCF_S1_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S1_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S1_Trig, LOW);
  Sonar[0] = pulseIn(PCF_S1_Echo, HIGH)*0.034/2;
}
void update_S2() {
  pcf8574.digitalWrite(PCF_S2_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S2_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S2_Trig, LOW);
  Sonar[1] = pulseIn(PCF_S2_Echo, HIGH)*0.034/2;
}
void update_S3() { 
  pcf8574.digitalWrite(PCF_S3_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S3_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S3_Trig, LOW);
  Sonar[2] = pulseIn(PCF_S3_Echo, HIGH)*0.034/2;
}

//Record sonar measurements and stringifies it
void update_SONAR_DATA() {
  update_S1();
  update_S2();
  update_S3();
  Sonar_Data["Sonar_1"] = Sonar[0];
  Sonar_Data["Sonar_2"] = Sonar[1];
  Sonar_Data["Sonar_3"] = Sonar[2];
  serializeJsonPretty(Sonar_Data, Sonar_Json);
}

void flash_code_handler (int code[], unsigned Size) {
  if (Size % 2 != 0) {Serial.println("code length error"); return;}
  for (int i = 0; i < Size; i = i + 2) {
    if (code[i] == 1) flash_LED_1(code[i+1]);
    else if (code[i] == 2) flash_LED_2(code[i+1]);
    else if (code[i] == 3) onboard_Red_LED(code[i+1]);
  }
}
#endif
