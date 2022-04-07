#ifndef ADS1115_H
#define ADS1115_H
//Dependencies
#include <Adafruit_ADS1X15.h>
#include <ArduinoJson.h>

//Create ADS object
Adafruit_ADS1115 ADS;
//12C Address on ADS when ADDR pin is GND
uint8_t ADSaddr = 0x48;

//Array for holding ADS data
float ADS_DATA[3] = {0, 0, 0};
//JSON object for storing data; change size to accomodate any needed data
StaticJsonDocument<500> ADS_Data; 
//Stringify JSON for setting over websockets
char ADS_Json[200]; 

//Initialization ADS function
bool ADS_Setup(TwoWire T) {
  ADS_Data["type"] = "sensor";
  ADS_Data["Name"] = "ADS";
  if (!ADS.begin(ADSaddr, &T)) return false; //Confirm Connection
  return true;
}

//Self-explanatory ADS data acqusition functions
void update_Lumosity() { //Stable measurments???? Need cap?
  ADS_DATA[0] = ADS.readADC_SingleEnded(0);//ADS.computeVolts(ADS.readADC_SingleEnded(0));
}
void update_Bat_Volt() {
  ADS_DATA[1] = 4*ADS.computeVolts(ADS.readADC_SingleEnded(1));
}
void update_Audio() { //Only Audio is integer data
  ADS_DATA[2] = ADS.readADC_SingleEnded(2);
}


//Record ADS measurements and stringifies it
void update_ADS() {
//  Serial.println("ADS");
//  update_Lumosity();
//  update_Bat_Volt();
//  update_Audio();
  ADS_Data["Lumosity"] = ADS_DATA[0];
  ADS_Data["Bat_Volt"] = ADS_DATA[1];
  ADS_Data["Audio"] = ADS_DATA[2];
  serializeJson(ADS_Data, ADS_Json);
  delay(1000);
}

#endif
