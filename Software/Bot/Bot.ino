//Dependencies
#include <Wire.h>
#include "ADS1115.h"
#include "MCP.h"
#include "MPU.h"
#include "Servo.h"
#include "Motor.h"
#include "WebSocket.h"

//I2C
//Redefine SCL/SDA pins
#define I2C_SCL 16
#define I2C_SDA 0
//Specify I2C pins for initializing other I2C devices
TwoWire I2C = TwoWire(0);

//Motor Speeds; sent from server / GUI
String input_Spd[2] = {"0", "0"};

//Dual-Core; cannot connect to server and run motors / sensors
TaskHandle_t DualTask;
void DualTaskcode( void * pvParameters ){
  USE_SERIAL.print("Task1 running on core ");
  USE_SERIAL.println(xPortGetCoreID());

  for(;;){
    //Constantly poll and send sensor data to server
    handle_Motor(input_Spd[0], input_Spd[1]); 
    update_Pos_Rot();
    webSocket.sendTXT(MPU_Json);
    update_ADS();
    webSocket.sendTXT(ADS_Json);
    update_SONAR_DATA();
    webSocket.sendTXT(Sonar_Json);
    delay(10);
  }
}

//WSHandler
void evaluateWSMsg(String& str) {
  unsigned int size = str.length();
  char buffer[size]; str.toCharArray(buffer, size);
  DynamicJsonDocument server_JSON(1024);
  DeserializationError err = deserializeJson(server_JSON, buffer);
  if (err) {USE_SERIAL.println("Error reading server_JSON"); return;}
  if (server_JSON["request"] == "N/A") {} //All server JSON must contain a request and instruction
  else if (server_JSON["request"] == "GET_MPU") {
    webSocket.sendTXT(MPU_Json);
    USE_SERIAL.println(MPU_Json);
  }
  else if (server_JSON["request"] == "GET_ADS") {
    webSocket.sendTXT(ADS_Json);
    USE_SERIAL.println(ADS_Json);
  }
  else if (server_JSON["request"] == "GET_SONAR") {
    webSocket.sendTXT(Sonar_Json);
    USE_SERIAL.println(Sonar_Json);
  }
    
  if (server_JSON["instruction"] == "N/A") {}
  else if (server_JSON["instruction"] == "LEFT_MOTOR") {
  input_Spd[0] = static_cast<int>(server_JSON["LEFT_MOTOR_SPEED"]);
  }
  else if (server_JSON["instruction"] == "RIGHT_MOTOR") {
  input_Spd[1] = static_cast<int>(server_JSON["RIGHT_MOTOR_SPEED"]);
  }

  str = "";
}

void setup() {
  //I2C
  I2C.begin(I2C_SDA, I2C_SCL, 100000);

  //Setup All Devices
  MCP_setup(I2C);
  ADS_Setup(I2C);
  MPU_setup(I2C);
  servo_Setup();
  Motor_Setup();
  configCamera();
  xTaskCreatePinnedToCore(
    DualTaskcode, // Function to implement the task
    "DualTask",   // Name of the task 
    10000,        // Stack size in words 
    NULL,         // Task input parameter 
    0,            // Priority of the task 
    NULL,         // Task handle. 
    0);           // Core where the task should run 

  //Connect to WiFi
  WSSetup(SSID, password, server_IP, port);
}

void loop() {
  webSocket.loop();
  evaluateWSMsg(WSMsg);
  liveCam();
}
