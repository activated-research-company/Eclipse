#ifndef SetpointController_h
#define SetpointController_h

#include "screen.h"
  
class SetpointController {
  public:
    SetpointController(double* setpoint, double deltaValue, double minValue, double maxValue, int outputPin, Screen* screen);
    void IncrementSetpoint(void (*delayRoutine)(int), bool (*breakWhenTrue)());
    void DecrementSetpoint(void (*delayRoutine)(int), bool (*breakWhenTrue)());
  private:
    double* _setpoint;
    double _deltaValue;
    double _minValue;
    double _maxValue;
    int _outputPin;
    Screen* _screen;
    void ChangeSetpoint(double deltaValue, double stopOnValue, void (*delayRoutine)(int), bool (*breakWhenTrue)());
    void WriteSetpointToNVM();
};

#endif
