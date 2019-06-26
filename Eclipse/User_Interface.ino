void get_info() {

  Analog_Tests();
  screen->ShowDiagnostics(Adj_Current, Volts, Power, Resistance);

  while (true) {
    if (PushButtonTwoIsDepressed()) {
      break;
    }
  }
}

void set_Setpoint() {
  screen->ShowSetpointMenu(Setpoint);
  while (true) {
    switch (GetNextButtonPress(3, &Run_PID)) {
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

void UpdateTemperature(bool showStar) {
    if ((Temperature != last_temperature)) { // update screen only if data has changed to reduce flicker
    
    screen->UpdateTemperature(last_temperature, Temperature);
    last_temperature = Temperature;

    if (showStar) {
      if (Output > 0) {
        screen->AddTemperatureStar();
      } else {
        screen->RemoveTemperatureStar();
      }
    }
  }
}

void ReadAndUpdateTemperature() {
  read_temp(true);
  UpdateTemperature(false);
  watchdogTimer->Refresh();
}
