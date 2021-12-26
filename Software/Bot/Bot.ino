//Libraries 
#include <Wire.h>
#include "ADS1115.h"
#include "MCP.h"
#include "MPU.h"
#include "Servo.h"
#include "Motor.h"
#include "WebSocket.h"

//I2C
#define I2C_SCL 16
#define I2C_SDA 0
TwoWire I2C = TwoWire(0);

//Incoming-Speed
String input_Spd[2] = {"0", "0"};

//Dual-Core
TaskHandle_t DualTask;

void DualTaskcode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    handle_Motor(input_Spd[0], input_Spd[1]); 
    update_Pos_Rot();
    update_ADS();
    update_SONAR_DATA();
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
  if (server_JSON["request"] == "N/A") {}
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
  input_Spd[0] = server_JSON["LEFT_MOTOR_SPEED"];
  }
  else if (server_JSON["instruction"] == "RIGHT_MOTOR") {
  input_Spd[1] = server_JSON["RIGHT_MOTOR_SPEED"];
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
  if(connected) liveCam();
}
