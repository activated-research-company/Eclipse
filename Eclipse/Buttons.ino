#define PowerButtonPin 12
#define PushButtonOnePin 2 // push button naming is left to right
#define PushButtonTwoPin 3
#define PushButtonThreePin 4

bool PowerButtonIsDepressed() {
  return digitalRead(PowerButtonPin) == 0;
}

bool PowerButtonIsNotDepressed() {
  return !PowerButtonIsDepressed();
}

bool PushButtonOneIsDepressed() {
  return digitalRead(PushButtonOnePin) == 0;
}

bool PushButtonOneIsNotDepressed() {
  return !PushButtonOneIsDepressed();
}

bool PushButtonTwoIsDepressed() {
  return digitalRead(PushButtonTwoPin) == 0;
}

bool PushButtonTwoIsNotDepressed() {
  return !PushButtonTwoIsDepressed();
}

bool PushButtonThreeIsDepressed() {
  return digitalRead(PushButtonThreePin) == 0;
}

bool PushButtonThreeIsNotDepressed() {
  return !PushButtonThreeIsDepressed();
}

bool AtLeastOneButtonIsDepressed() {
  return PushButtonOneIsDepressed() || PushButtonTwoIsDepressed() || PushButtonThreeIsDepressed();
}

int GetNextButtonPress(int validButtons, void (*loopRoutine)()) {
  while (AtLeastOneButtonIsDepressed()) {
    watchdogTimer->Delay(1);
  }
  watchdogTimer->Delay(5); // filteres out additional responses right before going from depressed to not depressed
  while (true) {
    if (PowerButtonIsNotDepressed()) { TurnOff(); return PowerButton; }
    if (validButtons >= 1 and PushButtonOneIsDepressed()) { return PushButtonOne; }
    if (validButtons >= 2 and PushButtonTwoIsDepressed()) { return PushButtonTwo; }
    if (validButtons >= 3 and PushButtonThreeIsDepressed()) { return PushButtonThree; }
    if (loopRoutine) { (*loopRoutine)(); }
    watchdogTimer->Delay(1);
  }
}
