void All_Keys_False() {
    KEY1 = false;
    KEY2 = false;
    KEY3 = false;
}

void get_info() {

  Analog_Tests();
  screen->ShowDiagnostics(Adj_Current, Volts, Power, Resistance, kp, ki, kd);

  while (AtLeastOneButtonIsDepressed()){}

  while (true) {
    if (PushButtonTwoIsDepressed()) { break; }
  }
}

void wait_4_key_press() {
  
  while (AtLeastOneButtonIsDepressed()) { Run_PID(); }
  
  All_Keys_False();
  
  while (true){

    if (PowerButtonIsDepressed()) {
      TurnOff();
      break;
    }
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
    
    Run_PID(); // prevent runaway while waiting for keys    
  }
}

void set_Setpoint(){
  screen->ShowSetpointMenu(Setpoint);
  wait_4_key_press();
  while (true) {
    while (PushButtonOneIsDepressed()) {
      setpointController->IncrementSetpoint(&delay_WDT, &PushButtonOneIsNotDepressed);
    }
    while (PushButtonTwoIsDepressed()) {
      setpointController->DecrementSetpoint(&delay_WDT,&PushButtonTwoIsNotDepressed);
    }
    if (PushButtonThreeIsDepressed()) { break; }
  }
  while (AtLeastOneButtonIsDepressed()) // wait for all keys up
  All_Keys_False();
}

void set_Kp()
{
  screen->ShowPidComponentMenu("Kp", kp);
  while (true) {
    wait_4_key_press();
    if (KEY1) {
      if (kp <= 200) { kp += 1; }
    }
    if (KEY2) {
      if (kp > 0) { kp -= 1; }
    }
    if (KEY3) { break; }
  }
}

void set_Ki(){
  while (true) {
    screen->ShowPidComponentMenu("Ki", ki);
    wait_4_key_press();
    if (KEY1) { ki += 1; }
    if (KEY2) {
      if (ki > 0) { ki = ki - 1; }
    }
    if (KEY3) { break; }
  }
}

void set_Kd(){
  while (true) {
    screen->ShowPidComponentMenu("Kd", kd);        
    wait_4_key_press();
    if (KEY1) { kd += 1; }
    if (KEY2) {
      if (kd > 0) { kd -= 1; }
    }
    if (KEY3) { break; }
  }
}

void Edit_Parameters(){

  Serial.println("Edit Param Enter");

EP_Start:
   
  screen->ShowPidMenu();
    while (AtLeastOneButtonIsDepressed()){ // wait for all keys up
        Run_PID();   // prevent runaway while waiting for keys
    }

    int iconCtrl = 0;
  
    for(;;){
        Run_PID();   // prevent runaway while waiting for keys
  
        if (PushButtonOneIsDepressed()){
            iconCtrl = (iconCtrl +  1) % 4;
            switch(iconCtrl) {
              case 0:
                screen->ShowTriangleOne();
              case 1:
                screen->ShowTriangleTwo();
              case 2:
                screen->ShowTriangleThree();
              case 3:
                screen->ShowTriangleFour();
            }
                      
            delay_WDT(250);
        }if (PushButtonTwoIsDepressed()){
            switch(iconCtrl){
                case 0:
                    set_Kp();
                    break;
                case 1:
                    set_Ki();
                    break;
                case 2:
                    set_Kd();
                    break;
                case 3:
                    get_info();
                    break;
                default:
                    screen->ShowMain(Setpoint, Temperature);
                    break;
            }
            goto EP_Start;      // redraw and start over
        }if (PushButtonThreeIsDepressed()){
            screen->ShowMain(Setpoint, Temperature);
            return;
        }
    }
}
