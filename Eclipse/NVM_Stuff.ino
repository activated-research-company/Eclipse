void NVM_Print() {
  if (DEBUG_MODE_IS_ON) {
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
}

int Write_NVM() {
  Serial.print("NVM writer receiving setpoint ");
  Serial.println(Setpoint, 1);  
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

  NVM_Print();
  
  if (EEPROM.read(NVM_Flag) != 0xAA) { // data does not exist in non-volatile memory
    Setpoint = Default_Setpoint;
    kp = Default_Kp;
    ki = Default_Ki;
    kd = Default_Kd;
    InFilt_State = Default_InFilt;
    OutFilt_State = Default_OutFilt;
    EEPROM.write(NVM_Flag, 0xAA);       // Set the Flag for next time
    Write_NVM();
  }
  else {
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
