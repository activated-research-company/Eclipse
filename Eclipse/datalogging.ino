void print_datalog_data()
{
  #ifdef DEBUG_MODE
    Serial.print(Unfiltered_Temp, 2);
    Serial.print(",");
    Serial.print(Delta_T);
    Serial.print(",");
    Serial.print(Power_Avg);
    Serial.print(",");
    Serial.print(Test_Heater_Fast());
    Serial.print(",");
    Serial.print(Setpoint, 1);
    Serial.print(",");
    Serial.print(error, 1);
    Serial.print(",");
    Serial.print(fault, 1);
    Serial.print(",");
    Serial.print(Fault_Count, 1);
    Serial.print(",");
    Serial.print(state, 1);
    Serial.println();
  #endif
}
