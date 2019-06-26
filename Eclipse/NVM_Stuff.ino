#define Default_Kp 100
#define Default_Ki 0
#define Default_Kd 0
#define Default_Setpoint 305
#define Default_InFilt 0
#define Default_OutFilt 0

#define NVM_Flag 1
#define NVM_Setpt 2
#define NVM_Kp 4
#define NVM_Ki 5
#define NVM_Kd 6
#define NVM_InFilt 7
#define NVM_OutFilt 8

void NVM_Print() {
  #ifdef DEBUG_MODE
    Serial.print("NVM_Flag = ");
    Serial.println(EEPROM.read(NVM_Flag));
    Serial.print("NVM_SetPt = ");
    int a = EEPROM.read(NVM_Setpt);
    int b = EEPROM.read(NVM_Setpt + 1);
    int temp = a * 256 + b;
    Serial.println(temp);
    Serial.print("NVM_InFilt = ");
    Serial.println(EEPROM.read(NVM_InFilt));
    Serial.print("NVM_OutFilt = ");
    Serial.println(EEPROM.read(NVM_OutFilt));
  #endif
}

int Write_NVM() {
  EEPROM.write(NVM_OutFilt, OutFilt_State);
  EEPROM.write(NVM_InFilt, InFilt_State);
}

int Recall_NVM() {

  NVM_Print();
  
  if (EEPROM.read(NVM_Flag) != 0xAA) { // variables do not exist in non-volatile memory
    Setpoint = Default_Setpoint;
    InFilt_State = Default_InFilt;
    OutFilt_State = Default_OutFilt;
    EEPROM.write(NVM_Flag, 0xAA); // set the Flag for next time
    Write_NVM();
  }
  else { // variables found
    int a = EEPROM.read(NVM_Setpt);
    int b = EEPROM.read(NVM_Setpt + 1);
    Setpoint = (a * 256 + b) / 10.0;
    InFilt_State = EEPROM.read(NVM_InFilt);
    OutFilt_State = EEPROM.read(NVM_OutFilt);
  }
  
  NVM_Print();
}
