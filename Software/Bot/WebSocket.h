#ifndef WEBSOCKET_H
#define WEBSOCKET_H
////Utilize http://www.iotsharing.com/2020/03/demo-48-using-websocket-for-camera-live.html

//Dependencies
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

//DEBUG 
#define DEBUG true

//Server Msg

//WS objects
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//WIFI Info
const char* SSID = "BadWifi";
const char* password = "Sanmina02";
const char* server_IP = "192.168.0.20";
int port = 80;

//Specify serial port
#define USE_SERIAL Serial

//Camera
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
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void liveCam(){
  //capture a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Frame buffer could not be acquired");
      return;
  }
  //Send binary camera data to server
  webSocket.sendBIN(fb->buf, fb->len);

  //return the frame buffer back to be reused
  esp_camera_fb_return(fb);
}

//Motor Speeds; sent from server / GUI
String input_Spd[2] = {"0", "0"};

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0) {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

//WS handler function that reacts to different WS events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {

    //Let the user know that WS is disconnected
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;

    //Let the user know that WS is connected
    case WStype_CONNECTED: {
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
      //Let server know ESP32 is connecting
      StaticJsonDocument<200> ESP32_Profile;
      ESP32_Profile["type"] = "master-device";
      ESP32_Profile["Name"] = "ESP32-CAM";
      ESP32_Profile["message"] = "trying to connect...";
      char Txt[150]; serializeJson(ESP32_Profile, Txt);
      webSocket.sendTXT(Txt);
      if (DEBUG) USE_SERIAL.println(Txt);}
      break;

    //Let Arduino program know if there is WS Message
    case WStype_TEXT: {
      StaticJsonDocument<400> server_JSON;
      char buffer[300];
      sprintf(buffer, "%s\n", payload);
      if (DEBUG) USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      
      //If sent data isn't JSON, ERROR
      DeserializationError error = deserializeJson(server_JSON, buffer);
      if (error) {USE_SERIAL.println("Error reading server_JSON"); break;}

      if (server_JSON["type"] == "motor") {
        String temp_left = server_JSON["left"]; input_Spd[0] = String(temp_left);
        String temp_right = server_JSON["right"]; input_Spd[1] = String(temp_right);
      }
      break;
    }

    //Call the hexdump function.
    case WStype_BIN:
      if (DEBUG) USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
      break;

    //Different types of WS events that must be specified to avoid error
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

//WS Setup
void Wsetup() {
  USE_SERIAL.begin(115200);
  
  USE_SERIAL.setDebugOutput(true);

  for(uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  if (DEBUG) {
    USE_SERIAL.println("SSID: " + String(SSID));
    USE_SERIAL.println("password: " + String(password));
    USE_SERIAL.println("server_IP: " + String(server_IP));
    USE_SERIAL.println("port: " + String(port));
  }
  WiFiMulti.addAP(SSID, password);

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    USE_SERIAL.println("CONNECTING");
    delay(100);
  }

  // server address, port and URL
  webSocket.begin(server_IP, port, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

}

#endif
