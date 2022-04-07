//Dependencies
#include <Wire.h>
#include "ADS1115.h"
#include "PCF.h"
#include "MPU.h"
#include "Servo.h"
#include "Motor.h"
#include "WebSocket.h"

bool connected = false;
//WB Clock
unsigned long Clock;

//Dual-Core; cannot connect to server and run motors / sensors
//TaskHandle_t DualTask;
void DualTaskcode( void * pvParameters ){
  USE_SERIAL.print("Task1 running on core ");
  USE_SERIAL.println(xPortGetCoreID());

  for(;;){
    //USE_SERIAL.println("Second core breathing");
    flash_LED_2(100);
    //Continously update motor speed
    if (connected) {
      handle_Motor(input_Spd[0], input_Spd[1]); 
      //Constantly poll and send sensor data to server
      update_Pos_Rot();
      update_ADS(); //retest when ads comes home
      update_SONAR_DATA();
    }
    delay(100);
  }
}

void setup() {
  //I2C
  I2C.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.begin(115200);

  //Onboard LED
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  Sonar_Data["type"] = "sensor";
  Sonar_Data["Name"] = "Sonar";
  pcf8574.pinMode(PCF_S1_Trig, OUTPUT); 
  pcf8574.pinMode(PCF_S1_Echo, INPUT);
  pcf8574.pinMode(PCF_S2_Trig, OUTPUT); 
  pcf8574.pinMode(PCF_S2_Echo, INPUT);
  pcf8574.pinMode(PCF_S3_Trig, OUTPUT); 
  pcf8574.pinMode(PCF_S3_Echo, INPUT);
  pcf8574.pinMode(PCF_LED_1, OUTPUT, LOW);
  pcf8574.pinMode(PCF_LED_2, OUTPUT, LOW);
  if (pcf8574.begin()){
    for (int i = 0; i < 5; i++) {
      flash_LED_1(100);
  }};
  //Setup All Devices
  delay(1000);
  ADS_Data["type"] = "sensor";
  ADS_Data["Name"] = "ADS";
  if (ADS.begin(ADSaddr, &I2C)) for (int i = 0; i < 5; i++) flash_LED_1(100); //Confirm Connection
//  if (ADS_Setup(I2C)) for (int i = 0; i < 5; i++) onboard_Red_LED(100);
//  if (!MPU_setup(I2C)) for (int i = 0; i < 5; i++) onboard_Red_LED(1000);
//  delay(1000);

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
//
//  //Connect to WiFi
  WiFiMulti.addAP(SSID, password);

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    onboard_Red_LED(50);
  } for (int i = 0; i < 5; i++) flash_LED_1(100);

  // server address, port and URL
  webSocket.begin(server_IP, port, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  webSocket.sendTXT("ESP32 has connected!");
  connected = true;
}

void loop() {
  webSocket.loop();
  webSocket.sendTXT(MPU_Json);
  webSocket.sendTXT(Sonar_Json);
  webSocket.sendTXT(ADS_Json);
  liveCam();
}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++)
  {
    I2C.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (I2C.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      int tally = String(i, DEC).toInt();
      for (int i = 0; i < tally; i++) onboard_Red_LED(500);
        onboard_Red_LED(500); 
      delay(1500);
      count++;
    }
  }
  Serial.print ("Found ");  
  for (int i = 0; i < String(count, DEC).toInt(); i++) onboard_Red_LED(2000);    
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}
