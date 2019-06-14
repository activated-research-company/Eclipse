void get_info() {

  Analog_Tests();
  screen->ShowDiagnostics(Adj_Current, Volts, Power, Resistance, kp, ki, kd);

  while (AtLeastOneButtonIsDepressed()){}

  while (true) {
    if (PushButtonTwoIsDepressed()) { break; }
  }
}

void set_Setpoint() {
  screen->ShowSetpointMenu(Setpoint);
  while (true) {
    while (PushButtonOneIsDepressed()) {
      setpointController->IncrementSetpoint(&delay_WDT, &PushButtonOneIsNotDepressed);
    }
    while (PushButtonTwoIsDepressed()) {
      setpointController->DecrementSetpoint(&delay_WDT,&PushButtonTwoIsNotDepressed);
    }
    if (PushButtonThreeIsDepressed()) { break; }
  }
  while (AtLeastOneButtonIsDepressed()) { }
}

void Edit_Parameters(){

EP_Start:
   
  screen->ShowPidMenu();
  while (AtLeastOneButtonIsDepressed()){ Run_PID(); } // prevent runaway while waiting for keys

  int iconCtrl = 0;

  while (true) {
    Run_PID();   // prevent runaway while waiting for keys

    if (PushButtonOneIsDepressed()){
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
                  
      delay_WDT(250);
    }
    
    if (PushButtonTwoIsDepressed()) {
      switch(iconCtrl){
        case 0:
          setPidValue("Kp", &kp);
          break;
        case 1:
          setPidValue("Ki", &ki);
          break;
        case 2:
          setPidValue("Kd", &kd);
          break;
        case 3:
          get_info();
          break;
        default:
          screen->ShowMain(Setpoint, Temperature);
          break;
      }
      goto EP_Start;
    }
    
    if (PushButtonThreeIsDepressed()) {
      screen->ShowMain(Setpoint, Temperature);
      return;
    }
  }
}

void setPidValue(char* pidComponent, double *pidValue) {
  screen->ShowPidComponentMenu(pidComponent, *pidValue);
  double oldValue;
  while (true) {
    oldValue = *pidValue; 
    switch (GetNextButtonPress()) {
      case PushButtonOne: *pidValue += 1; break;
      case PushButtonTwo: if (*pidValue > 0) { *pidValue -= 1; } break;
      case PushButtonThree: { return; }
    }
    screen->UpdatePidComponentValue(pidComponent, oldValue, *pidValue);
    Write_NVM();
  }
}
