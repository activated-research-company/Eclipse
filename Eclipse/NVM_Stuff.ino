// From TD Merge
// 10/9/18 fixed NVM recall user interface for CC

void NVM_Print() {
  Serial.print("NVM_Flag = ");
  Serial.println(EEPROM.read(NVM_Flag) );

  Serial.print("NVM_Kp = ");
  Serial.println(EEPROM.read(NVM_Kp) );

  Serial.print("NVM_Ki = ");
  Serial.println(EEPROM.read(NVM_Ki) );

  Serial.print("NVM_Kd = ");
  Serial.println(EEPROM.read(NVM_Kd) );

  Serial.print("NVM_SetPt = ");
  int a = EEPROM.read(NVM_Setpt);
  int b = EEPROM.read(NVM_Setpt + 1);
  int temp = a * 256 + b;
  Serial.println(temp);

  Serial.print("NVM_InFilt = ");
  Serial.println(EEPROM.read(NVM_InFilt) );

  Serial.print("NVM_OutFilt = ");
  Serial.println(EEPROM.read(NVM_OutFilt) );

  Serial.println();
}

void Param_Print()
{
  if (DEBUG_MODE_IS_ON) {
    Serial.print("Kp = ");
    Serial.println(kp, 4);
    Serial.print("Ki = ");
    Serial.println(ki, 4);
    Serial.print("Kd = ");
    Serial.println(kd, 4);
    Serial.print("SetPt = ");
    Serial.println(Setpoint, 4);
    Serial.println();
  }
}


int Write_NVM() {
  int temp = Setpoint;   // convert double to single int (assume setpoint is < 65536)
  int a = highByte(temp);
  int b = lowByte(temp);
  EEPROM.write(NVM_Setpt, a);
  EEPROM.write(NVM_Setpt + 1, b); // Setpoint gets 16 bits
  EEPROM.write(NVM_Kp, kp);
  EEPROM.write(NVM_Ki, ki);
  EEPROM.write(NVM_Kd, kd);
  EEPROM.write(NVM_OutFilt, OutFilt_State);
  EEPROM.write(NVM_InFilt, InFilt_State);
}

int Recall_NVM() {

  // tft.setRotation(1);
  // tft.fillScreen(ILI9341_BLACK);
  // tft.setCursor(0, 0);
  // tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  // tft.println("NVM Recall");
  Serial.println("NVM Recall");
  NVM_Print();
  if (EEPROM.read(NVM_Flag) != 0xAA) {  // See if there is valid data in EEPROM
    tft.fillScreen(bg_color);
    tft.setCursor(00, 50);
    tft.println("  NVM Restore");
    tft.println("  Press Any Key");
     wait_4_key_press();
     tft.fillScreen(bg_color);
    tft.setCursor(00, 50);
    tft.println("  ! NVM Restored !");
    delay_WDT(1000);
    tft.fillScreen(bg_color);
    tft.drawBitmap(50, 0, arcLogo, 256, 256, 0x001F);  // This is the ARC Logo
    strobe_WDT();

    Serial.println("Bad Data, Restoring to Defaults");
    Setpoint = Default_Setpoint;    // if not, resore defaults
    kp = Default_Kp;
    ki = Default_Ki;
    kd = Default_Kd;
    InFilt_State = Default_InFilt;
    OutFilt_State = Default_OutFilt;
    EEPROM.write(NVM_Flag, 0xAA);       // Set the Flag for next time
    Write_NVM();
  }
  else
  {
    //   tft.println("Good Data");
    Serial.println("Good Data");

    int a = EEPROM.read(NVM_Setpt);
    int b = EEPROM.read(NVM_Setpt + 1);
    Setpoint = a * 256 + b;
    kp = EEPROM.read(NVM_Kp);
    ki = EEPROM.read(NVM_Ki);
    kd = EEPROM.read(NVM_Kd);
    InFilt_State = EEPROM.read(NVM_InFilt);
    OutFilt_State = EEPROM.read(NVM_OutFilt);

  }
  NVM_Print();
}
