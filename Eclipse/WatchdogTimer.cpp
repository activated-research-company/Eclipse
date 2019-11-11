#include "WatchdogTimer.h"
#include <Arduino.h>

WatchdogTimer::WatchdogTimer(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  _state = false;
}

void WatchdogTimer::Refresh() {
  _state = !_state;
  digitalWrite(_pin , _state);
}

void WatchdogTimer::Delay(unsigned long milliseconds) {
  for (int ticks = 0; ticks < milliseconds; ticks++) {
    delay(1);
    Refresh();
  }
}
