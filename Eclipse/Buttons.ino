#define PowerButtonPin 12

// push button naming is left to right
#define PushButtonOnePin 2
#define PushButtonTwoPin 3
#define PushButtonThreePin 4

bool PowerButtonIsDepressed() {
  return digitalRead(PowerButtonPin) == 0;
}

bool PushButtonOneIsDepressed() {
  return digitalRead(PushButtonOnePin) == 0;
}

bool PushButtonTwoIsDepressed() {
  return digitalRead(PushButtonTwoPin) == 0;
}

bool PushButtonThreeIsDepressed() {
  return digitalRead(PushButtonThreePin) == 0;
}

bool AtLeastOneButtonIsDepressed() {
  return PowerButtonIsDepressed() || PushButtonOneIsDepressed() || PushButtonTwoIsDepressed() || PushButtonThreeIsDepressed();
}
