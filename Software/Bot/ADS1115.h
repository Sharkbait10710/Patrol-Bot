#ifndef ADS1115_H
#define ADS1115_H
#include <Adafruit_ADS1X15.h>
#include <ArduinoJson.h>

Adafruit_ADS1115 ADS;
uint8_t ADSaddr = 0x48;
float ADS_DATA[3] = {0, 0, 0};
StaticJsonDocument<3> ADS_Data; //Change size depending on how much data you want to send
char ADS_Json[100]; 
void ADS_Setup(TwoWire T) {
  if (!ADS.begin(ADSaddr, &T)) {} //Confirm Connection
}

void update_Lumosity() {
  ADS_DATA[0] = ADS.computeVolts(ADS.readADC_SingleEnded(0));
}

void update_Bat_Volt() {
  ADS_DATA[1] = 4*ADS.computeVolts(ADS.readADC_SingleEnded(1));
}

void update_Audio() {
  ADS_DATA[2] = ADS.computeVolts(ADS.readADC_SingleEnded(2));
}

void update_ADS() {
  update_Lumosity();
  update_Bat_Volt();
  update_Audio();
  ADS_Data["Lumosity"] = ADS_DATA[0];
  ADS_Data["Bat_Volt"] = ADS_DATA[1];
  ADS_Data["Audio"] = ADS_DATA[2];
  serializeJson(ADS_Data, ADS_Json);
}

#endif
