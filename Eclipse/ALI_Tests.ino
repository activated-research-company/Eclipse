void ALI_Tests() { // test push buttons, display, RTD, heater, current sensor, WDT
  
  while (true) {
    
    screen->ShowAliTestMenu();
  
    wait_4_key_press();
    
    while (true) {
      if (KEY1) { Heater_Test(); }
      if (KEY2) { Display_Test(); }
      if (KEY3) { break; }
    }
    
    screen->ShowMoreAliTestMenu();
    
    wait_4_key_press();
    while (true) {
      if (KEY1) { WDT_Test(); }
      if (KEY2) {
          screen->ShowGoodbye();
          delay_WDT(500);
          return;
        }
      if (KEY3) { break; }
    }
  }
}

void Display_Test() {
  
  screen->ShowScreenTestMenu();
  
  wait_4_key_press();
  
  while (true) {
    if (KEY1) {
      delay_WDT(6500);
      screen->Test();
      break;
    }
    if (KEY2) { break; }
    if (KEY3) { break; }
  }
}

void Heater_Test() {
  
  screen->ShowHeaterTestMenu();
  
  wait_4_key_press();
  while (true) {
    if (KEY1) { Htr_Test(); }
    if (KEY2) { return; }
    if (KEY3) { return; }
  }  
}
  
void WDT_Test() {
  
    screen->ShowWatchdogTestMenu();
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
          
      }
      if (KEY2) { return; }
      if (KEY3) { return; }
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
    delay_WDT(3000);                    // hold power so we can read meter
    analogWrite(FET_Pin, 0);            // Turn off Heater
    read_Volts();
  
    calc_MidPt();
    calc_Adj_Current();   // Calculate Adjusted current by subtracting midpoint .. sets Mid_Point variable
    calc_Power();         // Sets Power variable
    calc_Resistance();    // Sets Resistance variable

    read_temp(); 
    screen->ShowHeaterTestDriveResults(drive, Temperature, Volts, Adj_Current, Power, Resistance);  
    wait_4_key_press();
    
    if (KEY1) { 
      if (drive <= 200) { drive += 50; }
    }
    if (KEY2) {
        if (drive >= 50)
          drive = drive - 50;
    }
    if (KEY3) { return; }
    
  }
  
  analogWrite(FET_Pin, 0);
}



void log_htr_current() {

  while(true)
  {
    for (int j = 0; j <= 255; j = j + 25) {
        analogWrite(FET_Pin, j);            // Set Heater
        read_temp();                              // Read the RTD
        Unfiltered_Temp = Temperature;  
        delay(30);
        Serial.print("  Output= ");   Serial.print(j);
        Serial.print("  RTD= ");      Serial.print(Unfiltered_Temp, 2);
        Serial.print("  Htr I= ");    Serial.println(Test_Heater_Fast());
        while (AtLeastOneButtonIsDepressed()) { // wait for all keys up
        }
        while (true)
        {
            if (PushButtonOneIsDepressed()) {
              KEY1 = true;
              break;
            }
            if (PushButtonTwoIsDepressed()) {
              KEY2 = true;
              break;
            }
            if (PushButtonThreeIsDepressed()) {
              KEY3 = true;
              break;
            }
        }
    }
  }
}
  
