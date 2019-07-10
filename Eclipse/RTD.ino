#include "Filter.h"
ExponentialFilter<double> temperatureFilter(30, 0);

double lastSetpoint = 0;
bool currentlyFilteringTemperature = false;

void read_temp() {

  RawTemperature = GetRawTemperature();
  Temperature = RawTemperature;

  if (Setpoint != lastSetpoint) {
    lastSetpoint = Setpoint;
    currentlyFilteringTemperature = false;
  }

  if (!currentlyFilteringTemperature && Setpoint > 1 && Temperature > Setpoint) {
    temperatureFilter.SetCurrent(Temperature);
    currentlyFilteringTemperature = true;
  }

  if (currentlyFilteringTemperature) {
    temperatureFilter.Filter(Temperature);
    Temperature = temperatureFilter.Current(); 
  }  
}

double GetRawTemperature() {
  uint16_t rtd = max.readRTD();
  double rawTemperature = max.temperature(100, RREF);
  uint8_t fault = max.readFault();
  RTD_Fault = fault;
  if (fault) { LogFault(fault); }
  return rawTemperature;
}

void LogFault(int fault) {
  #ifdef DEBUG_MODE
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
  #endif
  max.clearFault();  
}
