void ALI_Tests() { // test push buttons, display, RTD, heater, current sensor, WDT
  screen->ShowAliTestMenu();

  switch (GetNextButtonPress(3, &Run_PID)) {
    case PushButtonOne: Heater_Test(); break;
    case PushButtonTwo: Display_Test(); break;
    case PushButtonThree: break;
  }
  
  screen->ShowMoreAliTestMenu();
  
  switch (GetNextButtonPress(3, &Run_PID)) {
    case PushButtonOne: WDT_Test(); break;
    case PushButtonTwo: ALI_Tests(); break;
    case PushButtonThree: break;
  }
}

void Display_Test() {
  screen->ShowScreenTestMenu();
  switch (GetNextButtonPress(3, &Run_PID)) {
    case PushButtonOne:
      watchdogTimer->Delay(6500);
      screen->Test();
      break;
    case PushButtonTwo: break;
    case PushButtonThree: break;
  }
}

void Heater_Test() {
  screen->ShowHeaterTestMenu();
  switch (GetNextButtonPress(3, &Run_PID)) {
    case PushButtonOne: Htr_Test(); break;
    case PushButtonTwo: return;
    case PushButtonThree: return;
  }  
}
  
void WDT_Test() {
  screen->ShowWatchdogTestMenu();
  switch (GetNextButtonPress(3, &Run_PID)) {
    case PushButtonOne : while (true) { }
    case PushButtonTwo: return;
    case PushButtonThree: return;
  }
}

void Htr_Test() {
  
  int drive;

  analogReference(INTERNAL2V56);      // use 2.56 volts as reference

  analogWrite(FET_Pin, 0); // turn off heater to read zero current
  calc_MidPt();
  drive = 0;
  
  while (true)
  {
    screen->ShowHeaterTestDriveHeader(drive);
  
    analogWrite(FET_Pin, drive);          // Turn on Heater so we get full current
    read_Current();
    watchdogTimer->Delay(3000);                    // hold power so we can read meter
    analogWrite(FET_Pin, 0);            // Turn off Heater
    read_Volts();
  
    calc_MidPt();
    calc_Adj_Current();   // Calculate Adjusted current by subtracting midpoint .. sets Mid_Point variable
    calc_Power();         // Sets Power variable
    calc_Resistance();    // Sets Resistance variable

    read_temp(); 
    screen->ShowHeaterTestDriveResults(drive, Temperature, Volts, Adj_Current, Power, Resistance);  
    
    switch (GetNextButtonPress(3, &Run_PID)) {
      case PushButtonOne: if (drive <= 200) { drive += 50; } break;
      case PushButtonTwo: if (drive >= 50) { drive = drive - 50; } break;
      case PushButtonThree: return;
    }
  }
  
  analogWrite(FET_Pin, 0);
}
  
