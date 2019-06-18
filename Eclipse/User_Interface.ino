void get_info() {

  Analog_Tests();
  screen->ShowDiagnostics(Adj_Current, Volts, Power, Resistance, kp, ki, kd);

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

void Edit_Parameters(){

  screen->ShowPidMenu();
  
  int iconCtrl = 0;

  while (true) {
    switch (GetNextButtonPress(3, &Run_PID)) {
      case PushButtonOne:
        iconCtrl = (iconCtrl +  1) % 4;
        switch(iconCtrl) {
          case 0:
            screen->ShowTriangleOne();
            break;
          case 1:
            screen->ShowTriangleTwo();
            break;
          case 2:
            screen->ShowTriangleThree();
            break;
          case 3:
            screen->ShowTriangleFour();
            break;
        }
        break;
      case PushButtonTwo:
        switch(iconCtrl){
          case 0:
            setPidValue("Kp", &kp);
            screen->ShowPidMenu();
            break;
          case 1:
            setPidValue("Ki", &ki);
            screen->ShowPidMenu();
            break;
          case 2:
            setPidValue("Kd", &kd);
            screen->ShowPidMenu();
            break;
          case 3:
            get_info();
            screen->ShowPidMenu();
            break;
        }
        break;
      case PushButtonThree: return;
     }
  }
}

void setPidValue(char* pidComponent, double *pidValue) {
  screen->ShowPidComponentMenu(pidComponent, *pidValue);
  double oldValue;
  while (true) {
    oldValue = *pidValue; 
    switch (GetNextButtonPress(3, &Run_PID)) {
      case PushButtonOne: *pidValue += 1; break;
      case PushButtonTwo: if (*pidValue > 0) { *pidValue -= 1; } break;
      case PushButtonThree: { return; }
    }
    screen->UpdatePidComponentValue(pidComponent, oldValue, *pidValue);
    Write_NVM();
  }
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
  read_temp();
  UpdateTemperature(false);
  watchdogTimer->Refresh();
}
