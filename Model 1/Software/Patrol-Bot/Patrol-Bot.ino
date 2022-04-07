//[WebSockets]
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
const char* SSID = "BadWifi";
const char* password = "Sanmina02";
const char* server_IP = "192.168.0.11";
int port = 80;
//Handle Binary Data
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0) {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}
//Handle Websocket events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      //Send Connected_JSON to Server
      char Connected_JSON[] =   "{\"type\": \"master-device\","
                                "\"Name\": \"ESP32-CAM\","
                                "\"message\": \"trying to connect...\"}";
      webSocket.sendTXT(Connected_JSON);
      break;}
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

//[Camera]
#define sensor_t sensor_t_
#include "esp_camera.h" 
#undef sensor_t
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
void configCamera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 9;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}
//develop the camera frames and send to server
void liveCam(){
  //capture a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      //Serial.println("Frame buffer could not be acquired");
      return;
  }
  //Send binary camera data to server
  webSocket.sendBIN(fb->buf, fb->len);

  //return the frame buffer back to be reused
  esp_camera_fb_return(fb);
}

//[I2C]
#include <Wire.h>
TwoWire I2C = TwoWire(0);
uint8_t I2C_SCL = 16;
uint8_t I2C_SDA = 0;

//[PCF]
#include "PCF8574.h"
uint8_t PCFaddr = 0x20;
PCF8574 pcf8574(&I2C, PCFaddr, I2C_SDA, I2C_SCL);
//Identify all hardware pins
#define PCF_S1_Trig P0
#define PCF_S1_Echo P1
#define PCF_S2_Trig P2
#define PCF_S2_Echo P3
#define PCF_S3_Trig P6
#define PCF_S3_Echo P7
#define PCF_LED_1 P4
#define PCF_LED_2 P5
float Sonar[3] = {0, 0, 0};
//Emit propogating sound waves for measuring distance (cm) using HC-SR04
float update_S1() { //Distance in cm
  pcf8574.digitalWrite(PCF_S1_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S1_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S1_Trig, LOW);
  Sonar[0] = pulseIn(PCF_S1_Echo, HIGH)*0.034/2;
  return Sonar[0];
}
float update_S2() {
  pcf8574.digitalWrite(PCF_S2_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S2_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S2_Trig, LOW);
  Sonar[1] = pulseIn(PCF_S2_Echo, HIGH)*0.034/2;
  return Sonar[1];
}
float update_S3() { 
  pcf8574.digitalWrite(PCF_S3_Trig, LOW);
  delayMicroseconds(2);
  pcf8574.digitalWrite(PCF_S3_Trig, HIGH);
  delayMicroseconds(10);
  pcf8574.digitalWrite(PCF_S3_Trig, LOW);
  Sonar[2] = pulseIn(PCF_S3_Echo, HIGH)*0.034/2;
  return Sonar[2];
}
//Record sonar measurements and stringifies it
String SONAR_JSON = "";
void update_Sonar_Data() {
  SONAR_JSON = "{\"type\": \"sensor\",";
  SONAR_JSON += "\"Name\": \"Sonar\",";
  SONAR_JSON += "\"Sonar_1\": " + String(update_S1()) + ",";
  SONAR_JSON += "\"Sonar_2\": " + String(update_S2()) + ",";
  SONAR_JSON += "\"Sonar_3\": " + String(update_S3()) + "}";
}

//LED
void onboard_Red_LED(int T) {
  digitalWrite(33, LOW);  delay(T);
  digitalWrite(33, HIGH); delay(T);
}
void onboard_White_LED(int T) {
  digitalWrite(4, HIGH);  delay(T);
  digitalWrite(4, LOW); delay(T);
}
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

//[ADS]
#include <Adafruit_ADS1X15.h>
//Create ADS object
Adafruit_ADS1115 ADS;
//12C Address on ADS when ADDR pin is GND
uint8_t ADSaddr = 0x48;
//Array for holding ADS data
float ADS_DATA[3] = {0, 0, 0};
//Self-explanatory ADS data acqusition functions
float update_Lumosity() { //Stable measurments???? Need cap?
  ADS_DATA[0] = ADS.readADC_SingleEnded(0);//ADS.computeVolts(ADS.readADC_SingleEnded(0));
  return ADS_DATA[0];
}
float update_Bat_Volt() {
  ADS_DATA[1] = 4*ADS.computeVolts(ADS.readADC_SingleEnded(1));
  return ADS_DATA[1];
}
float update_Audio() { //Only Audio is integer data
  ADS_DATA[2] = ADS.readADC_SingleEnded(2);
  return ADS_DATA[2];
}
//Record ADS measurements and stringifies it
String ADS_JSON = "";
void update_ADS() {
  ADS_JSON = "{\"type\": \"sensor\",";
  ADS_JSON += "\"Name\": \"ADS\",";
  ADS_JSON += "\"Lumosity\": " + String(update_Lumosity()) + ",";
  ADS_JSON += "\"Bat_Volt\": " + String(update_Bat_Volt()) + ",";
  ADS_JSON += "\"Audio\": " + String(update_Audio()) + "}";
}

//[MPU]
#include <Adafruit_MPU6050.h>
#include <BasicLinearAlgebra.h>
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
String MPU_JSON = "";
void update_Pos_Rot() {
  poll();
  MPU_JSON = "{\"type\": \"sensor\",";
  MPU_JSON += "\"Name\": \"MPU-6050\",";
  MPU_JSON += "\"delta_S.x\": " + String(delta_S[0]) + ",";
  MPU_JSON += "\"delta_S.y\": " + String(delta_S[1]) + ",";
  MPU_JSON += "\"delta_S.z\": " + String(delta_S[2]) + ",";
  MPU_JSON += "\"delta_R.x\": " + String(delta_R[0]) + ",";
  MPU_JSON += "\"delta_R.y\": " + String(delta_R[1]) + ",";
  MPU_JSON += "\"delta_R.z\": " + String(delta_R[2]) + "}";
}

//Dual-Core; cannot connect to server and run motors / sensors
//TaskHandle_t DualTask;
bool connected = false;
int Time;
void DualTaskcode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    Serial.println("Second core breathing");
    flash_LED_2(100);
    //Continously update motor speed
    if (connected) {
      //handle_Motor(input_Spd[0], input_Spd[1]); 
      //Constantly poll and send sensor data to server
      Serial.println("Sent data");
      update_Pos_Rot();
      update_ADS();
      update_Sonar_Data();
    }
    delay(100);
  }
}

bool DEBUG = true;

void setup() {
  //[Setup]: Serial
  if (DEBUG) {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
  }

  //Onboard Red LED: HIGH = LV
  pinMode(33, OUTPUT); digitalWrite(33, HIGH);

  //Onboard White LED: HIGH = HV
  pinMode(4, OUTPUT); digitalWrite(4, LOW);
  
  //[Setup]: PCF8574
//  pcf8574.pinMode(PCF_S1_Trig, OUTPUT); 
//  pcf8574.pinMode(PCF_S1_Echo, INPUT);
//  pcf8574.pinMode(PCF_S2_Trig, OUTPUT); 
//  pcf8574.pinMode(PCF_S2_Echo, INPUT);
//  pcf8574.pinMode(PCF_S3_Trig, OUTPUT); 
//  pcf8574.pinMode(PCF_S3_Echo, INPUT);
//  pcf8574.pinMode(PCF_LED_1, OUTPUT, LOW);
//  pcf8574.pinMode(PCF_LED_2, OUTPUT, LOW);
//  if (pcf8574.begin()) for (int i = 0; i < 5; i++)
//    onboard_White_LED(100); else for (int i = 0; i < 5; i++) onboard_White_LED(1000);
//  delay(2000);
  //[Setup]: ADS1115
//  if (ADS.begin(ADSaddr, &I2C)) for (int i = 0; i < 5; i++)
//    onboard_White_LED(100); else  onboard_White_LED(1000);
//  delay(2000);
  //[Setup]: MPU
//  if (MPU.begin(MPUaddr, &I2C)) for (int i = 0; i < 5; i++)
//    onboard_Red_LED(100); else onboard_Red_LED(1000);
//  MPU.setAccelerometerRange(MPU6050_RANGE_8_G);
//  MPU.setGyroRange(MPU6050_RANGE_500_DEG);
//  MPU.setFilterBandwidth(MPU6050_BAND_21_HZ);
//  MPU.getEvent(&a, &g, &temp);
  
//  //[Setup]: Camera
//  configCamera();
  
//  //[Setup]: WebSocket
//  if (DEBUG) for(uint8_t t = 4; t > 0; t--) {
//    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
//    Serial.flush();
//    delay(1000);
//  }
//  WiFiMulti.addAP(SSID, password);
//  //WiFi.disconnect();
//  while(WiFiMulti.run() != WL_CONNECTED) {
//    Serial.println("ATTEMPTING TO CONNECT");
//    delay(100);
//  }
//  // server address, port and URL
//  webSocket.begin(server_IP, port, "/");
//  // event handler
//  webSocket.onEvent(webSocketEvent);
//  // try ever 5000 again if connection has failed
//  webSocket.setReconnectInterval(1000);

  Time = millis();
  connected = false;

  xTaskCreatePinnedToCore(
  DualTaskcode, // Function to implement the task
  "DualTask",   // Name of the task 
  10000,        // Stack size in words 
  NULL,         // Task input parameter 
  0,            // Priority of the task 
  NULL,         // Task handle. 
  0);           // Core where the task should run 
}

void loop() {
  onboard_Red_LED(1000);
  Scanner();
//  webSocket.loop();
//  webSocket.sendTXT(MPU_JSON);
//  webSocket.sendTXT(ADS_JSON);
//  webSocket.sendTXT(SONAR_JSON);
//  liveCam();
}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  onboard_Red_LED(100);
  I2C.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      for (int j = 0; j < i; j++) onboard_White_LED(100);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
      delay(1000);
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}
