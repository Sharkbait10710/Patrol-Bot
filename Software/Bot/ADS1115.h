#ifndef ADS1115_H
#define ADS1115_H
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ADS;
uint8_t ADSaddr = 0x48;
float ADS_DATA[3] = [0, 0, 0];

void ADS_Setup(TwoWire T) {
  if (!ADS.begin(ADSaddr, &T)) {} //Confirm Connection
}

float get_Lumosity() {
  return ADS.computeVolts(ADS.readADC_SingleEnded(0));
}

float get_Bat_Volt() {
  return 4*ADS.computeVolts(ADS.readADC_SingleEnded(1));
}

float get_Audio() {
  return ADS.computeVolts(ADS.readADC_SingleEnded(2));
}

#endif
