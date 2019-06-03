void ALI_Tests()
{

// Test Pushbuttons, Display, RTD, Heater, Current Sensor, WDT
//

 if ((digitalRead(PB1) == 0) && (digitalRead(PB3) ==0))   // Do this if PB 1 and PB3 are down on power up
 {

    while (true)   // Do this until we bail and return
        {
        
              tft.setRotation(1);
              
              tft.fillScreen(ILI9341_BLACK);
              tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
              tft.setCursor(SCol1, SLine1);
              tft.print("ALI Tests");
              
              tft.setCursor(SCol1, SLine1+50);
              tft.println("1 = Heater    ");
              tft.setCursor(SCol1, SLine1+80);
              tft.println("2 = Display       ");
              tft.setCursor(SCol1, SLine1+110);
              tft.println("3 = More       ");
              tft.setCursor(SCol1, SLine4);
              tft.print(" 1      2      3 ");
            
              wait_4_key_press();
              while (true) {
                if (KEY1) 
                    {
                         Heater_Test();
                    }
                if (KEY2)
                    {
                        Display_Test();
                    }
                if (KEY3)
                    break;
              } // WT
              
              tft.fillScreen(ILI9341_BLACK);
              tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
              tft.setCursor(SCol1, SLine1);
              tft.print("ALI Tests");
              
              tft.setCursor(SCol1, SLine1+50);
              tft.println("1 = Watch Dog    ");
              tft.setCursor(SCol1, SLine1+80);
              tft.println("2 = More  ");
              tft.setCursor(SCol1, SLine1+110);
              tft.println("3 = Exit        ");
              tft.setCursor(SCol1, SLine4);
              tft.print(" 1      2      3 ");
              
              wait_4_key_press();
              while (true) {
                if (KEY1) 
                    {
                        WDT_Test();
                    }
                if (KEY2)
                    {
                        tft.fillScreen(ILI9341_BLACK);
                        tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
                        tft.setCursor(SCol1, SLine1);
                        tft.print("Goodbye");
                        delay_WDT(500);
                        return;
                    }
                if (KEY3)
                    break;
              } // WT     
    }
 }
}




void Display_Test()
{
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    tft.setCursor(SCol1, SLine1);
    tft.print("Display Test");
    tft.setCursor(SCol1, SLine2);


    tft.setCursor(SCol1, SLine1+50);
    tft.println("1 = Run        ");
    tft.setCursor(SCol1, SLine1+80);
    tft.println("2 =            ");
    tft.setCursor(SCol1, SLine1+110);
    tft.println("3 = Exit       ");
    tft.setCursor(SCol1, SLine4);
    tft.print(" 1      2      3 ");
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
           G_Test();
           break;
      }
      if (KEY2)
      {
          break;
      }
      if (KEY3)
          break;
    }
    tft.setRotation(1);    // restore 
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    return;
  
}

void Pushbutton_Test()
{
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    tft.setCursor(SCol1, SLine1);
    tft.print("Pushbutton Test");
      tft.setCursor(SCol1, SLine1+50);
    tft.println("1 = Run        ");
      tft.setCursor(SCol1, SLine1+80);
    tft.println("2 =            ");
      tft.setCursor(SCol1, SLine1+110);
    tft.println("3 = Exit       ");
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
          tft.fillScreen(ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
          tft.setCursor(SCol1, SLine1);
          tft.print("Press 1");
          wait_4_key_press();
          if (KEY1)
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("Got it !");
              delay_WDT(500);
          }
          else
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("!!! Wrong !!!");
              delay_WDT(500);
          }
          
          tft.fillScreen(ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
          tft.setCursor(SCol1, SLine1);
          tft.print("Press 2");
          wait_4_key_press();
          if (KEY2)
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("Got it !");
              delay_WDT(500);
          }
          else
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("!!! Wrong !!!");
              delay_WDT(500);
          }

          tft.fillScreen(ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
          tft.setCursor(SCol1, SLine1);
          tft.print("Press 3");
          wait_4_key_press();
          if (KEY3)
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("Got it !");
              delay_WDT(500);
          }
          else
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("!!! Wrong !!!");
              delay_WDT(500);
          }

          tft.fillScreen(ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
          tft.setCursor(SCol1, SLine1);
          tft.print("Press On/Off");
          while (digitalRead(PBOn) == 0)  
          {
            // wait for PB not pressed
            strobe_WDT();
          }
          while (digitalRead(PBOn) == 1)  
          {
             // Wait for press
             strobe_WDT();
          }
          {
              tft.setCursor(SCol1, SLine2);
              tft.print("Got it !");
              delay_WDT(500);
          }
          
          
      }
      if (KEY2)
      {
          return;
      }
      if (KEY3)
          return;
    }
  
}


void RTD_Test()
{
  
  
  tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    tft.setCursor(SCol1, SLine1);
    tft.print("RTD Test");
      tft.setCursor(SCol1, SLine1+50);
    tft.println("1 = Run        ");
      tft.setCursor(SCol1, SLine1+80);
    tft.println("2 =            ");
      tft.setCursor(SCol1, SLine1+110);
    tft.println("3 = Exit       ");
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
        tft.setCursor(SCol1, SLine4);
        tft.print("            Exit");  ;
        
        read_temp();  

        tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(4);         
        tft.setCursor(LCol1, LLine2);
        tft.print("Tmp= ");
        tft.setCursor(LCol2, LLine2);
        tft.print(Temperature, 1);

        last_temperature = Temperature;
        
        while(true)
        {
            tft.setCursor(SCol1, SLine1);
            read_temp();  
            if (Temperature != last_temperature)      // Update TFT only if data has changed to reduce flicker
            {
                tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(4);
                tft.setCursor(LCol2, LLine2);
                tft.print(last_temperature, 1);
                tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(4);
                tft.setCursor(LCol2, LLine2);
                tft.print(Temperature, 1);
                last_temperature = Temperature;
            }
            if (digitalRead(PB3) == 0)
               break;
        }
             
 
        return;
      }

      if (KEY2)
          return;
      if (KEY3)
          return;
    }
  
}
  

void Heater_Test()
{
  tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    tft.setCursor(SCol1, SLine1);
    tft.print("Heater Test");
      tft.setCursor(SCol1, SLine1+50);
    tft.println("1 = Run        ");
      tft.setCursor(SCol1, SLine1+80);
    tft.println("2 =            ");
      tft.setCursor(SCol1, SLine1+110);
    tft.println("3 = Exit       ");
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
         Htr_Test();
      }
      if (KEY2)
      {
          return;
      }
      if (KEY3)
          return;
    }
  
}
  

void WDT_Test()
{
  tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
    tft.setCursor(SCol1, SLine1);
    tft.print("Watch Dog Test");
      tft.setCursor(SCol1, SLine1+50);
    tft.println("1 = Run        ");
      tft.setCursor(SCol1, SLine1+80);
    tft.println("2 =            ");
      tft.setCursor(SCol1, SLine1+110);
    tft.println("3 = Exit       ");
  
    wait_4_key_press();
    while (true) {
      if (KEY1) 
      {
          
      }
      if (KEY2)
      {
          return;
      }
      if (KEY3)
          return;
    }
  
}

void Htr_Test()
{
  int drive;

  analogReference(INTERNAL2V56);      // Use 2.56 volts as reference

  analogWrite(FET_Pin, 0);          // Turn off htr to read zero current
  calc_MidPt();
  drive = 0;
  
  while (true)
  {
        tft.setRotation(1);
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.println();
        tft.println("  *** Testing ***");
        tft.print  ("     Drive = ");
        tft.println(drive);
    
        analogWrite(FET_Pin, drive);          // Turn on Heater so we get full current
        read_Current();
        delay_WDT(3000);                    // hold power so we can read meter
        analogWrite(FET_Pin, 0);            // Turn off Heater
        read_Volts();

        calc_MidPt();
        calc_Adj_Current();   // Calculate Adjusted current by subtracting midpoint .. sets Mid_Point variable
        calc_Power();         // Sets Power variable
        calc_Resistance();    // Sets Resistance variable
        
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
   
        tft.println();
        read_temp(); 

        tft.print("  Results D = ");
        tft.println(drive);
        
        tft.print("  RTD   = ");
        tft.print(Temperature,0);
        tft.println(" Deg");
      
        
        
        tft.print("  Volts = ");
        tft.print(Volts,1);
        tft.println(" V");
      
        tft.print("  Curr  = ");
        tft.print(Adj_Current,0); //
        tft.println(" mA");
      
        tft.print("  Power = ");
        tft.print(Power,1);
        tft.println(" W");
      
        tft.print("  Htr R = ");
        tft.println(Resistance,0);
        // tft.println("Ohm");

      tft.setCursor(SCol1, SLine4);
      tft.print(" Inc   Dec  Exit ");   

      wait_4_key_press();
      if (KEY1) 
                { 
                  if (drive <= 200)
                      drive = drive + 50;
                }
                if (KEY2)
                {
                    if (drive >= 50)
                      drive = drive - 50;
                }
                if (KEY3)
                    return;
      
       
      
        
  }
analogWrite(FET_Pin, 0);
}



void log_htr_current()
{

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
    while ((digitalRead(PB1) == 0) || (digitalRead(PB2) == 0) || (digitalRead(PB3) == 0) ) // wait for all keys up
    {
    }
    while (true)
    {
        if (digitalRead(PB1) == 0)
        {
          KEY1 = true;
          break;
        }
        if (digitalRead(PB2) == 0)
        {
          KEY2 = true;
          break;
        }
        if (digitalRead(PB3) == 0)
        {
          KEY3 = true;
          break;
        }

    }

}
}
}
  
