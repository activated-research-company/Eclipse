#include "SetpointController.h"
#include "WatchdogTimer.h"
#include <EEPROM.h>

#define NVM_Setpt 2

SetpointController::SetpointController(double* setpoint, double deltaValue, double minValue, double maxValue, int outputPin, Screen* screen) {
  _setpoint = setpoint;
  _deltaValue = deltaValue;
  _minValue = minValue;
  _maxValue = maxValue;
  _outputPin = outputPin;
  _screen = screen;
}

void SetpointController::IncrementSetpoint(WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)()) {
  ChangeSetpoint(_deltaValue, _maxValue, watchdogTimer, (*breakWhenTrue));
}

void SetpointController::DecrementSetpoint(WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)()) {
  ChangeSetpoint(-_deltaValue, _minValue, watchdogTimer, (*breakWhenTrue));
}

void SetpointController::ChangeSetpoint(double deltaValue, double stopOnValue, WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)()) {

  analogWrite(_outputPin, 0);
  
  double previousSetpoint;
  int loops = 0;
  int multiplier = 1;
  
  while (true) {
    
    if ((*breakWhenTrue)()) { break; }

    if ((deltaValue > 0 and *_setpoint <= stopOnValue) or (deltaValue < 0 and *_setpoint >= stopOnValue)) {

      while ((deltaValue > 0 and *_setpoint + (deltaValue * multiplier) > stopOnValue) or
             (deltaValue < 0 and *_setpoint + (deltaValue * multiplier) < stopOnValue)) {
        // reduce loops and multiplier if deltaValue * multiplier will put us past stopOnValue
        if (multiplier == 1) { multiplier = 0; loops = 0; break; }
        if (multiplier == 10) { multiplier = 1; loops = 0; }
        if (multiplier == 50) { multiplier = 10; loops = 10; }
        if (multiplier == 100) { multiplier = 10; loops = 25; }
      }
            
      previousSetpoint = *_setpoint;

      if (multiplier > 1) {
         // scaling with a non-zero decimal part is ugly
         // it seems counterintuitive to go the opposite direction of the delta sign
         // but this flows better on the interface and would skip numbers otherwise
        if (deltaValue > 0) { *_setpoint = floor(*_setpoint); }
        if (deltaValue < 0) { *_setpoint = ceil(*_setpoint); }  
      }
      
      *_setpoint += deltaValue * multiplier;
      _screen->UpdateSetpoint(previousSetpoint, *_setpoint);

      loops++;
      
      if (loops < 10) {
      } else if (loops < 25) {        
        multiplier = 10;
      } else if (loops < 45) {
        multiplier = 50;
      } else {
        multiplier = 100;  
      }
      
      watchdogTimer->Delay(100);
    }
  }
  
  WriteSetpointToNVM();
  
}

void SetpointController::WriteSetpointToNVM() {
  int temp = *_setpoint * 10;
  int a = highByte(temp);
  int b = lowByte(temp);
  EEPROM.write(NVM_Setpt, a);
  EEPROM.write(NVM_Setpt + 1, b); // setpoint gets 16 bits
}
