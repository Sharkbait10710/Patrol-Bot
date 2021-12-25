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
    String IMU_data = get_Pos_Rot();
    delay(10);
  }
}

//WSHandler
void evaluateWSMsg(String& str) {
  if (Capt_Reg("GET_IMU", *str)) {
    USE_SERIAL.println("Turn stepper 1 OFF Continuously");
  }
  else if (Capt_Reg("GET_LUMOSITY", *str)) {
    USE_SERIAL.println("Turn stepper 1 CC Continuously");
  }
  else if (Capt_Reg("GET_BAT_VOLT", *str)) {
    USE_SERIAL.println("Turn stepper 1 C Continuously");
  }
  else if (Capt_Reg("GET_MIC", *str)) {
    USE_SERIAL.println("Turn stepper 2 OFF Continuously");
  }
  else if (Capt_Reg("GET_SONAR1", *str)) {
    USE_SERIAL.println("Turn stepper 2 CC Continuously");
  }
  else if (Capt_Reg("GET_SONAR2", *str)) {
    USE_SERIAL.println("Turn stepper 2 C Continuously");
  }
  else if (Capt_Reg("GET_SONAR3", *str)) {
    servo_pos = str.substring(16,str.length()).toInt();
    USE_SERIAL.println("Adjust servo to " + str.substring(16,str.length()));
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
  // put your main code here, to run repeatedly:

}
