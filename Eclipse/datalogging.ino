void print_debug_line(){
  if (DEBUG_MODE_IS_ON)
  {
    Serial.print("T= " + now);
    Serial.print("  TC= " + timeChange);
    Serial.print("  P_Tics= " + PID_Ticks);
    Serial.print("  RTD= ");
    Serial.print(Unfiltered_Temp);
    Serial.print("  DT= " + Delta_T);
    Serial.print("  PA= " + Power_Avg);
    Serial.print("  *** Curr = ");
    Serial.print(Current);
    Serial.print("    *** ");
    Serial.print(" Raw_Out= ");
    Serial.print(Unfiltered_Out);
    Serial.print(" Int_Out= " + Int_Out);
    Serial.println();
  }
}
    
void print_datalog_header()
{
  if (DEBUG_MODE_IS_ON)
  {
    Serial.print("Time");
    Serial.print(",");
    Serial.print("RTD"); 
    Serial.print(",");
    Serial.print("PWM");
    Serial.print(",");
    Serial.print("DT");  
    Serial.print(","); 
    Serial.print("PA"); 
    Serial.print(",");  
    Serial.print("Htr I"); 
    Serial.print(",");  
    Serial.print("Setpoint");
    Serial.print(",");
    Serial.print("Temp");
    Serial.print(",");
    Serial.print("Error");
    Serial.print(",");
    Serial.print("Kp");
    Serial.print(",");
    Serial.print("Ki");
    Serial.print(",");
    Serial.print("Kd");
    Serial.print(",");
    Serial.print("PTerm");
    Serial.print(",");
    Serial.print("ITerm");
    Serial.print(",");
    Serial.print("DTerm");
    Serial.print(",");
    Serial.print("Fault");
    Serial.print(",");
    Serial.print("FC");
    Serial.print(",");
    Serial.print("State");
    Serial.println();
  }
}

void print_datalog_data()
{
  if (DEBUG_MODE_IS_ON)
  {
    Serial.print(now / 1000, 1);
    Serial.print(",");
    Serial.print(Unfiltered_Temp, 2);
    Serial.print(",");
    Serial.print(Output, 1);
    Serial.print(",");
    Serial.print(Delta_T);
    Serial.print(",");
    Serial.print(Power_Avg);
    Serial.print(",");
    Serial.print(Test_Heater_Fast());
    Serial.print(",");
    Serial.print(Setpoint, 1);
    Serial.print(",");
    Serial.print(Input, 1);
    Serial.print(",");
    Serial.print(error, 1);
    Serial.print(",");
    Serial.print(kp, 1);
    Serial.print(",");
    Serial.print(ki, 1);
    Serial.print(",");
    Serial.print(kd, 1);
    Serial.print(",");
    Serial.print(PTerm, 1);
    Serial.print(",");
    Serial.print(ITerm, 1);
    Serial.print(",");
    Serial.print(DTerm, 1);
    Serial.print(",");
    Serial.print(fault, 1);
    Serial.print(",");
    Serial.print(Fault_Count, 1);
    Serial.print(",");
    Serial.print(state, 1);
    Serial.print("\n");
  }
}
