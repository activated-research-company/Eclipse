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
  return PowerButtonIsDepressed() || PushButtonOneIsDepressed() || PushButtonTwoIsDepressed() || PushButtonThreeIsDepressed();
}

int GetNextButtonPress() {
  while (AtLeastOneButtonIsDepressed()) { Run_PID(); }
  while (true) {
    if (PowerButtonIsDepressed()) { state = OFF; return PowerButton; }
    if (PushButtonOneIsDepressed()) { return PushButtonOne; }
    if (PushButtonTwoIsDepressed()) { return PushButtonTwo; }
    if (PushButtonThreeIsDepressed()) { return PushButtonThree; }
    Run_PID(); 
  }
}
