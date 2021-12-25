#ifndef MCP_H
#define MCP_H
#include <Adafruit_MCP23X17.h>

#define MCP_S1_Trig 8 
#define MCP_S1_Echo 9
#define MCP_S2_Trig 10
#define MCP_S2_Echo 11
#define MCP_S3_Trig 14
#define MCP_S3_Echo 15
#define MCP_LED_1 12
#define MCP_LED_2 13

Adafruit_MCP23X17 MCP;
uint8_t MCPaddr = 0x20;

void MCP_setup(TwoWire T) {
  if (!MCP.begin_I2C(MCPaddr, &T)) {} //Confirm Connection
  MCP.pinMode(MCP_S1_Trig, OUTPUT); 
  MCP.pinMode(MCP_S1_Echo, INPUT);
  MCP.pinMode(MCP_S2_Trig, OUTPUT); 
  MCP.pinMode(MCP_S2_Echo, INPUT);
  MCP.pinMode(MCP_S3_Trig, OUTPUT); 
  MCP.pinMode(MCP_S3_Echo, INPUT);
}

float read_S1() { //Distance in cm
  digitalWrite(MCP_S1_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S1_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S1_Trig, LOW);
  return pulseIn(MCP_S1_Echo, HIGH)*0.034/2;
}

float read_S2() { //Distance in cm
  digitalWrite(MCP_S2_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S2_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S2_Trig, LOW);
  return pulseIn(MCP_S2_Echo, HIGH)*0.034/2;
}

float read_S3() { //Distance in cm
  digitalWrite(MCP_S3_Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(MCP_S3_Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(MCP_S3_Trig, LOW);
  return pulseIn(MCP_S3_Echo, HIGH)*0.034/2;
}

void flash_LED_1(int T) {
  digitalWrite(MCP_LED_1, HIGH);
  delay(T);
  digitalWrite(MCP_LED_1, LOW);
}

void flash_LED_2(int T) {
  digitalWrite(MCP_LED_2, HIGH);
  delay(T);
  digitalWrite(MCP_LED_2, LOW);
}
#endif
