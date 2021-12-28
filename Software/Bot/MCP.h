#ifndef MCP_H
#define MCP_H
//Dependencies
#include <Adafruit_MCP23X17.h>
#include <ArduinoJson.h>

//Identify all hardware pins
#define MCP_S1_Trig 8 
#define MCP_S1_Echo 9
#define MCP_S2_Trig 10
#define MCP_S2_Echo 11
#define MCP_S3_Trig 14
#define MCP_S3_Echo 15
#define MCP_LED_1 12
#define MCP_LED_2 13

//Create MCP object
Adafruit_MCP23X17 MCP;
//12C Address on MCP when all address pins are GND
uint8_t MCPaddr = 0x20;

//Array for holding sonar distance data
float Sonar[3] = {0, 0, 0};
//JSON object for storing data; change size to accomodate any needed data
StaticJsonDocument<3> Sonar_Data; 
//Stringify JSON for setting over websockets
char Sonar_Json[100]; 

//Initialization MCP function
void MCP_setup(TwoWire T) {
  if (!MCP.begin_I2C(MCPaddr, &T)) {} //Confirm Connection
  MCP.pinMode(MCP_S1_Trig, OUTPUT); 
  MCP.pinMode(MCP_S1_Echo, INPUT);
  MCP.pinMode(MCP_S2_Trig, OUTPUT); 
  MCP.pinMode(MCP_S2_Echo, INPUT);
  MCP.pinMode(MCP_S3_Trig, OUTPUT); 
  MCP.pinMode(MCP_S3_Echo, INPUT);
}

//Emit propogating sound waves for measuring distance (cm) using HC-SR04
void update_S1() { //Distance in cm
  digitalWrite(MCP_S1_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S1_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S1_Trig, LOW);
  Sonar[0] = pulseIn(MCP_S1_Echo, HIGH)*0.034/2;
}
void update_S2() {
  digitalWrite(MCP_S2_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S2_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S2_Trig, LOW);
  Sonar[1] = pulseIn(MCP_S2_Echo, HIGH)*0.034/2;
}
void update_S3() { 
  digitalWrite(MCP_S3_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S3_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S3_Trig, LOW);
  Sonar[2] = pulseIn(MCP_S3_Echo, HIGH)*0.034/2;
}

//Record sonar measurements and stringifies it
void update_SONAR_DATA() {
  update_S1();
  update_S2();
  update_S3();
  Sonar_Data["Sonar_1"] = Sonar[0];
  Sonar_Data["Sonar_2"] = Sonar[1];
  Sonar_Data["Sonar_3"] = Sonar[2];
  serializeJson(Sonar_Data, Sonar_Json);
}

//Flash LEDs 
void flash_LED_1(int T) {
  digitalWrite(MCP_LED_1, HIGH);
  delay(T);
  digitalWrite(MCP_LED_1, LOW);
  delay(T);
}
void flash_LED_2(int T) {
  digitalWrite(MCP_LED_2, HIGH);
  delay(T);
  digitalWrite(MCP_LED_2, LOW);
  delay(T);
}
#endif
