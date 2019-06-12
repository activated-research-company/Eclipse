void read_Volts()
{
    Volts = 0;                          // Clear Volts
    for (int i = 0; i <= 255; i++) {    // Get the value of power supply voltage
      Volts = Volts + analogRead(1);
      delay(1);
      strobe_WDT();
    }
    Volts = Volts / 256;   // Average
    Volts = Volts / 20;    // Scale to Volts and use global variable
}

void read_Current()
{
  Current = 0;                        // Clear Average
  for (int i = 0; i <= 255; i++) {    // Get the value of full current
    Current += analogRead(0);
    delay(1);
    strobe_WDT();
  }
  Current = Current / 256;            // Calculate Average value  
  if (Current < 0) 
     Current = 0;
}

void calc_MidPt()  // Calculate the middle or zero point of the current sensor .. Must have heater off to do so
{
  Midpt = 0;                          // Clear Average
  for (int i = 0; i <= 255; i++) {    // Get the value of zero current
    Midpt = Midpt + analogRead(0);
    delay(1);
    strobe_WDT();
  }
  Midpt = Midpt / 256;  // Average and set global variable
}

void calc_Power()
  {
     Power = Volts * (Adj_Current / 1000);   // Calculate Power in Watts
  }
void calc_Resistance()
  {
    Resistance = Volts / (Adj_Current / 1000); // Calculate Heater Resistance in Ohms
  }

void calc_Adj_Current()
  {
    Serial.println(Midpt);
    Serial.println(Current);
    Adj_Current = (Midpt - Current) * 10; // Subtract from MidPt and Scale to mA
    if (Adj_Current < 0)
       Adj_Current = 0;
  }

int Analog_Tests()
{
  analogReference(INTERNAL2V56);      // Use 2.56 volts as reference
  analogWrite(FET_Pin, 0);            // Turn off Heater so we get no current

  Serial.println("Analog Tests  - Heater Off");
  analogWrite(FET_Pin, 0);
  delay_WDT(1000);

  read_Volts();  
  Serial.print("Volts = ");
  Serial.print(Volts);
  Serial.println(" V");

  calc_MidPt();

  analogWrite(FET_Pin, Max_Htr);          // Turn on Heater so we get full current
  read_Current();
  analogWrite(FET_Pin, 0);            // Turn off Heater

  calc_Adj_Current();   // Calculate Adjusted current by subtracting midpoint .. sets Mid_Point variable
  calc_Power();         // Sets Power variable
  calc_Resistance();    // Sets Resistance variable
  
  fault = 0;
  if (Volts > 50.0)
    bitSet(fault, 0);
  if (Volts < 44.0)
    bitSet(fault, 1);
  if (Current > 1500.0)
    bitSet(fault, 2);
  if (Resistance > 50.0)
    bitSet(fault, 3);
  if (Resistance < 20.0)
    bitSet(fault, 4);

  if (true)
  {
    Serial.println("Analog Test Results");
    Serial.print("Current = ");
    Serial.print(Adj_Current,0); 
    Serial.println(" mA");
    Serial.print("Volts = ");
    Serial.print(Volts);
    Serial.println(" V");
    Serial.print("Power = ");
    Serial.print(Power);
    Serial.println(" W");
    Serial.print("Resistance = ");
    Serial.print(Resistance);
    Serial.println(" Ohms");
    Serial.print("Fault = ");
    Serial.println(fault);
    Serial.println();
  }

  analogWrite(FET_Pin, 0);

  Serial.println("Analog Tests  - Done LUT Heater Off");

  return fault;

}

// A fast test to look for a gross over current in the heater
// This is a 'noisy' measurement and we only check for gross over current and lack of current
int Test_Heater_Fast()   
{
  int Fast_Current = analogRead(0);
  return Fast_Current;
}

void Test_Heater()
{
  // Verify heater is not pulling too much current
  // This is a 'noisy' measurement and we only check for gross over current and lack of current
  // Test for shorted heater ... If Adj_Current > 1500
  // Test for Open ... IF OP > 40 AND Adj_Current < 100

  int OP;
  read_Current();                 // faster version so we don't take too much time
  Adj_Current = (Midpt - Current) * 10; // Scale to mA
  OP = Output;                        // convert Double to Int
  HTR_Fault = 0;
  if (Adj_Current > 1500)             // test for over current
    HTR_Fault = 1;
  if ((OP > 40) && (Adj_Current < 100))  // test for under current
  {
  //  HTR_Fault = 2;   // disabled in version 17
  }
}

void Heater_PU_Test()
{
  analogReference(INTERNAL2V56);      // Use 2.56 volts as reference

  Serial.println("Heater Power Up Test");
  analogWrite(FET_Pin, 0); // Turn off Heater so we get no current
  delay_WDT(100);
  read_temp();
  Serial.print("Start Temperature = ");
  Serial.println(Temperature);

  read_Volts();
  Serial.print("Volts = ");
  Serial.print(Volts);
  Serial.println(" V");
  calc_MidPt();

  analogWrite(FET_Pin, Max_Htr); // Turn on Heater so we get full current
  read_Current();
  analogWrite(FET_Pin, 0);            // Turn off Heater

  calc_Adj_Current();   // Calculate Adjusted current by subtracting midpoint .. sets Mid_Point variable
  calc_Power();         // Sets Power variable
  calc_Resistance();    // Sets Resistance variable

  Serial.print("Volts = ");
  Serial.print(Volts);
  Serial.println(" V");
  Serial.print("Current = ");
  Serial.print(Adj_Current);
  Serial.println(" mA");
  Serial.print("Power = ");
  Serial.print(Power);
  Serial.println(" W");
  Serial.print("Resistance = ");
  Serial.print(Resistance);
  Serial.println(" Ohms");
  Serial.print("Mid Point = ");
  Serial.println(Midpt);

  read_temp();
  Serial.print("End Temperature = ");
  Serial.println(Temperature);
  HTR_Fault = 0;
  if (Power < 4) {
    HTR_Fault = 1;
    Serial.print("HTR_Fault=");
    Serial.println(HTR_Fault);
  }

  Serial.println("End Heater PU Test "); 
}
