//Libraries 
#include <Wire.h>
#include "ADS1115.h"
#include "MCP.h"
#include "MPU.h"
#include "Servo.h"
#include "Motor.h"
#include "WebSocket.h"
#include "Regex.h"

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
  if (Capt_Reg("GET_IMU", str)) {
    webSocket.sendTXT(IMU_DATA);
    USE_SERIAL.println(IMU_DATA);
  }
  else if (Capt_Reg("GET_ADS", str)) {
    webSocket.sendTXT(ADS_DATA);
    USE_SERIAL.println(IMU_DATA);
  }
  else if (Capt_Reg("GET_SONAR", str)) {
    webSocket.sendTXT(SONAR_DATA);
    USE_SERIAL.println(SONAR_DATA);
  }
  else if (Capt_Reg("LEFT_MOTOR", str)) {
    flush_Data();
    //Specify regex and extract data
  }
  else if (Capt_Reg("RIGHT_MOTOR", str)) {
    flush_Data();
    //Specify regex and extract data
  }
  else if (Capt_Reg("RIGHT_MOTOR", str)) {
    flush_Data();
    //Specify regex and extract data
  }
  flush_Data();
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
  xTaskCreatePinnedToCore(
    DualTaskcode, // Function to implement the task
    "DualTask",   // Name of the task 
    10000,        // Stack size in words 
    NULL,         // Task input parameter 
    0,            // Priority of the task 
    NULL,         // Task handle. 
    0);           // Core where the task should run 

  //Connect to WiFi
  WSSetup("BadWifi", "Sanmina02", "192.168.0.28", 8082);
}

void loop() {
  webSocket.loop();
  evaluateWSMsg(WSMsg);
}
