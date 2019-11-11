void ShowDiagnostics() {
  Analog_Tests();
  screen->ShowDiagnostics(Adj_Current, Volts, Power, Resistance, kp, ki, kd);
  GetNextButtonPress(1, NULL);
}

void set_Setpoint() {
  screen->ShowSetpointMenu(Setpoint);
  while (true) {
    switch (GetNextButtonPress(3, NULL)) {
      case PushButtonOne:
        while (PushButtonOneIsDepressed()) {
          setpointController->IncrementSetpoint(watchdogTimer, &PushButtonOneIsNotDepressed);
        }
        break;
      case PushButtonTwo:
        while (PushButtonTwoIsDepressed()) {
          setpointController->DecrementSetpoint(watchdogTimer, &PushButtonTwoIsNotDepressed);
        }
        break;
      case PushButtonThree: return;
      case PowerButton: TurnOff(); return;
    }
  }
  while (AtLeastOneButtonIsDepressed()) { }
}

void UpdateTemperature() {
    if (Temperature != LastTemperature) { // update screen only if data has changed to reduce flicker
      screen->UpdateTemperature(LastTemperature, Temperature);
      LastTemperature = Temperature;
  }
}

void ReadAndUpdateTemperature() {
  read_temp();
  UpdateTemperature();
  watchdogTimer->Refresh();
}
