#ifndef ADS1115_H
#define ADS1115_H
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ADS;
uint8_t ADSaddr = 0x48;
float ADS_data[3] = {0, 0, 0};
String ADS_DATA = "";
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
  String ret = "";
  for (int i = 0; i < 3; i++) 
    ret += String(ADS_data[i]) + "|";
  ADS_DATA = ret;
}

#endif
