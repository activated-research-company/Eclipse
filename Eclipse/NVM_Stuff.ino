#define Default_Setpoint 450

#define NVM_Flag 1
#define NVM_Setpt 2

void NVM_Print() {
  #ifdef DEBUG_MODE
    Serial.print("NVM_Flag = ");
    Serial.println(EEPROM.read(NVM_Flag));
    Serial.print("NVM_SetPt = ");
    int a = EEPROM.read(NVM_Setpt);
    int b = EEPROM.read(NVM_Setpt + 1);
    int temp = a * 256 + b;
    Serial.println(temp);
  #endif
}

int Recall_NVM() {

  NVM_Print();
  
  if (EEPROM.read(NVM_Flag) != 0xAA) { // variables do not exist in non-volatile memory
    Setpoint = Default_Setpoint;
    EEPROM.write(NVM_Flag, 0xAA); // set the Flag for next time
  }
  else { // variables found
    int a = EEPROM.read(NVM_Setpt);
    int b = EEPROM.read(NVM_Setpt + 1);
    Setpoint = (a * 256 + b) / 10.0;
  }

  Setpoint = round(Setpoint);
  
  NVM_Print();
}
