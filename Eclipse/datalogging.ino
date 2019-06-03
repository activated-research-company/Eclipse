 // From TD Merge
 
 void print_debug_line(){
      
      Serial.print("T= ");      Serial.print(now);
      Serial.print("  TC= ");  Serial.print(timeChange);
      Serial.print("  P_Tics= ");  Serial.print(PID_Ticks);
      Serial.print("  RTD= ");  Serial.print(Unfiltered_Temp, 2);
      Serial.print("  DT= ");   Serial.print(Delta_T);
      Serial.print("  PA= ");   Serial.print(Power_Avg);
      Serial.print("  *** Curr = ");   Serial.print(Current);
      
      
   
   //   Serial.print("  InFilt= ");  Serial.print(InFilt_State);
   //   Serial.print("  OutFilt= ");  Serial.print(OutFilt_State);
      
//      Serial.print("  Input= ");  Serial.print(Input, 2);
 //     Serial.print("  Set= "); Serial.print(Setpoint, 1);
 //     Serial.print("  Err= ");  Serial.print(error, 2);
   //   Serial.print("  PID= ");  Serial.print(kp, 0);
   //   Serial.print(" ");  Serial.print(ki, 0);
  //    Serial.print(" ");  Serial.print(kd, 0);
  //    Serial.print(" ");
  //    Serial.print(" PT= "); Serial.print(PTerm, 1);   
  //    Serial.print(" IT= "); Serial.print(ITerm, 1);
  //    Serial.print(" DT= "); Serial.print(DTerm, 1);
      Serial.print("    *** ");
      Serial.print(" Raw_Out= "); Serial.print(Unfiltered_Out,1);
      Serial.print(" Int_Out= "); Serial.print(Int_Out,1);
      Serial.println();
    }

    
void print_datalog_header()
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

void print_datalog_data()
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
  Serial.print(State, 1);
  Serial.print("\n");
}


