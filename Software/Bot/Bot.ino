//Dependencies
#include <Wire.h>
#include "ADS1115.h"
#include "PCF.h"
#include "MPU.h"
#include "Servo.h"
#include "Motor.h"
#include "WebSocket.h"

//WB Clock
unsigned long Clock;
//I2C
TwoWire I2C = TwoWire(0);

//Motor Speeds; sent from server / GUI
String input_Spd[2] = {"0", "0"};

//Dual-Core; cannot connect to server and run motors / sensors
//TaskHandle_t DualTask;
void DualTaskcode( void * pvParameters ){
  USE_SERIAL.print("Task1 running on core ");
  USE_SERIAL.println(xPortGetCoreID());

  for(;;){
    USE_SERIAL.println("Second core breathing");
    //Continously update motor speed
    handle_Motor(input_Spd[0], input_Spd[1]); 
    //Constantly poll and send sensor data to server
    update_Pos_Rot();
//    USE_SERIAL.println(MPU_Json);
//    webSocket.sendTXT(MPU_Json);
    //update_ADS(); //retest when ads comes home
    //webSocket.sendTXT(ADS_Json);
    update_SONAR_DATA();
//    USE_SERIAL.println(Sonar_Json);
//    webSocket.sendTXT(Sonar_Json);
    delay(10);
  }
}

//WSHandler
void evaluateWSMsg(String& str) {
//  else if (server_JSON["request"] == "GET_MPU") {
//    webSocket.sendTXT(MPU_Json);
//    USE_SERIAL.println(MPU_Json);
//  }
//  else if (server_JSON["request"] == "GET_ADS") {
//    webSocket.sendTXT(ADS_Json);
//    USE_SERIAL.println(ADS_Json);
//  }
//  else if (server_JSON["request"] == "GET_SONAR") {
//    webSocket.sendTXT(Sonar_Json);
//    USE_SERIAL.println(Sonar_Json);
//  }
//    
//  if (server_JSON["instruction"] == "N/A") {}
//  else if (server_JSON["instruction"] == "LEFT_MOTOR") {
//  input_Spd[0] = static_cast<int>(server_JSON["LEFT_MOTOR_SPEED"]);
//  }
//  else if (server_JSON["instruction"] == "RIGHT_MOTOR") {
//  input_Spd[1] = static_cast<int>(server_JSON["RIGHT_MOTOR_SPEED"]);
//  }

  str = "";
}

void setup() {
  //I2C
  I2C.begin(I2C_SDA, I2C_SCL, 100000);

  //Setup All Devices
  PCF_setup(I2C);
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
    1);           // Core where the task should run 

  //Connect to WiFi
  Serial.print("Connecting...");
  Wsetup();
//  Clock = millis();
}

void loop() {
  webSocket.loop();
  evaluateWSMsg(WSMsg);
//  if (millis()-Clock > 100) {
    //USE_SERIAL.println(MPU_Json);
  webSocket.sendTXT(MPU_Json);
    //USE_SERIAL.println(Sonar_Json);
  webSocket.sendTXT(Sonar_Json);
//    Clock = millis();
//  }
  liveCam();
}
