#ifndef SetpointController_h
#define SetpointController_h

#include "Screen.h"
#include "WatchdogTimer.h"
  
class SetpointController {
  public:
    SetpointController(double* setpoint, double deltaValue, double minValue, double maxValue, int outputPin, Screen* screen);
    void IncrementSetpoint(WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)());
    void DecrementSetpoint(WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)());
  private:
    double* _setpoint;
    double _deltaValue;
    double _minValue;
    double _maxValue;
    int _outputPin;
    Screen* _screen;
    void ChangeSetpoint(double deltaValue, double stopOnValue, WatchdogTimer* watchdogTimer, bool (*breakWhenTrue)());
    void WriteSetpointToNVM();
};

#endif
