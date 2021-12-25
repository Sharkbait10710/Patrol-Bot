#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include <Arduino.h>

//Libraries
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

//Wifi and Websocket objects
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//Set which serial channel to use
#define USE_SERIAL Serial

//String variable that is empty if WS message is N/A
String WSMsg = "";

//Prints out information during runtime if DEBUG is true
#define DEBUG false

//I think this function "clears" the memory buffer
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  if (DEBUG) USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0) {
      if (DEBUG) USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    if (DEBUG) USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  if (DEBUG) USE_SERIAL.printf("\n");
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
      String Txt = "Connected";
      webSocket.sendTXT(Txt);
      if (DEBUG) USE_SERIAL.println(Txt);}
      break;

    //Let Arduino program know if there is WS Message
    case WStype_TEXT: {
      char buffer[100];
      sprintf(buffer, "%s\n", payload);
      if (DEBUG) USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      WSMsg = buffer;
      break;

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
}

void WSSetup(const char* AP, const char* PW, const char* IP, int Port) {

  //Setup serial channel
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  if (DEBUG)
    for (unsigned i = 0; i < 3; i++)
     USE_SERIAL.println();

  //Flush out the buffer
  for(uint8_t t = 4; t > 0; t--) {
    if (DEBUG) USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    
    USE_SERIAL.flush();
    delay(1000);
  }

  //Connect to Wifi
  WiFiMulti.addAP(AP, PW);

  //Keep attempting Wifi Connection if disconnected
  while(WiFiMulti.run() != WL_CONNECTED)
    delay(100);

  // server address, port and URL
  webSocket.begin(IP, Port, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

}

#endif
