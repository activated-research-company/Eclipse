#include <Adafruit_MAX31865.h>
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include <EEPROM.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "screen.h"

#define DEBUG_MODE_IS_ON true
#define SKIP_TEST_MODE_IS_ON true

Adafruit_MAX31865 max = Adafruit_MAX31865(8, 7, 6, 5); // defines pins used for SPI: CS, DI, DO, CLK

#define RREF 430.0   // Rref value used with MAX31865 Chip

#define MANUAL 0      // For PID Loop 
#define AUTOMATIC 1   // For PID Loop

#define DIRECT 0      // For PID Loop
#define REVERSE 1     // For PID Loop

#define Max_Htr 200  // Limits the Heater PWM to this value to prevent over drive (maximum 255 = 90W)

//***** Hardware Pins Defined Here *****
#define FET_Pin  13   // Heater drive PWM using power FET

//Use these for the production units with a WDT. We remapped LEDB for use as the WDT strobe line
#define LEDA 46     // PWM-able and used for the power button ... 
#define WDT 45      // was LEDB on earlier versions remapped for WDT use

#define NVM_Flag 1
#define NVM_Setpt 2   // Setpoint gets two bytes for 16 bit save
#define NVM_Kp 4
#define NVM_Ki 5
#define NVM_Kd 6
#define NVM_InFilt 7
#define NVM_OutFilt 8

#define Default_Kp 100          // NVM limited to 8 bits for now
#define Default_Ki 0          // Gets copied into double later
#define Default_Kd 0
#define Default_Setpoint 305
#define Default_InFilt 0
#define Default_OutFilt 0

byte tt = 1; //for testing pwm
int HT = 0; // for use in debugging heater test
int k = 0;

int i;              // GP use in indexing loops
int x;              // GP use in loop indexes
int EEsize = 4096;  // size in bytes of mega board EEPROM

/*** Variables used in PID Loop ***/
unsigned long lastTime;   // used for integration time compute
unsigned long now;        // used for integration time compute
unsigned long timeChange;
int PID_Ticks = 0;            // increments on 500mS tick (if PID loop is 500mS)
bool refresh = 0;             // forces update of setpoint and temp after edit

int PWM_LUT[256];    // Use this to build a table of PWM vs Current Draw

double Input = 0;
double Output = 0;
double Setpoint;    // double is floating point variable
double PTerm, ITerm, DTerm, lastInput;
double kp = 50;
double ki = 13;
double kd = 255;
double error;
double Temperature;
double last_temperature;
double last_setpoint;
int RTD_Fault;                  // Gets set by RTD read function
int HTR_Fault;                  // Gets set by Test_Heater function
int Break_Code;

int Int_Out;   // used for casting float Outputs as integer
int Fault_Count = 0;

int Rate_Lim_Out = 0;
int Out_Step;

double Unfiltered_Temp;
double Filtered_Temp;
double Unfiltered_Out;
double Filtered_Out;
int Power_Avg;     // rolling average of power (drive) to FET
int Delta_T;       // Temp change over a 30 second interval

int SampleTime = 500;
double outMin, outMax;
bool inAuto = false;
int controllerDirection = DIRECT;

double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 10, consKi = 0.0, consKd = 0.0;

bool KEY1, KEY2, KEY3, KEY4; // Pushbutton keys

// Note int is 16 bit signed in Arduino
// Floats are 6-7 digits of resolution (32 bit)

int fault;        // This group used for analog tests
float Volts;
float Current;
float Adj_Current;
float Midpt;
float Av;
float Resistance;
float Power;

int set_on = 1;
double set_rem;

#define ST_SIZE 10            // This is the number of samples in the rolling average
static int ST[ST_SIZE];      // Rolling average filter for input RTD
static byte ST_index = 0;
static long ST_sum = 0;
static byte ST_count = 0;

#define SO_SIZE 10
static int SO[SO_SIZE];      // LastMeasurements
static byte SO_index = 0;
static long SO_sum = 0;
static byte SO_count = 0;

#define DT_SIZE 60            // This is the number of samples in delta T rolling avearge
static int DT[DT_SIZE];      // Rolling average filter for Delta T (Rate of Temp Change)
static byte DT_index = 0;
static long DT_sum = 0;
static byte DT_count = 0;

#define PA_SIZE 60            // This is the number of samples Power Average rolling avearge
static int PA[PA_SIZE];      // Rolling average filter Power Average
static byte PA_index = 0;
static long PA_sum = 0;
static byte PA_count = 0;

#define CA_SIZE 60            // This is the number of samples Current Average rolling avearge
static int CA[CA_SIZE];      // Rolling average filter Current Average
static byte CA_index = 0;
static long CA_sum = 0;
static byte CA_count = 0;

bool InFilt_State = 0;       // For Switching Filters in and out
bool OutFilt_State = 0;

bool WDT_State = 0;

int state;
#define OFF 0
#define ON 5
#define TEST 4
#define RUN 1
#define EDIT 2
#define SLEEP 3

Screen* screen;

void setup() {
  screen = new Screen(10, 9);
  Serial.begin(9600);
  max.begin(MAX31865_2WIRE); // RTD code and RTD Type
  SetControllerDirection(DIRECT);
  SetSampleTime(500); // loop rate (500 = 2 Hz)
  SetOutputLimits(0, 255); // min and max for output
  SetMode(AUTOMATIC);
  SetTunings(2, .5, 0); // initial tuning parameters - P, I, D
  pinMode(LEDA, OUTPUT);
  pinMode(WDT, OUTPUT);
  pinMode(FET_Pin, OUTPUT);
  InFilt_State = 0;
  OutFilt_State = 0; // int filter states, NVM recall will override later
  analogReference(INTERNAL2V56); // use 2.56 volts as reference
  analogWrite(FET_Pin, 0);
  state = OFF;
}

void TurnOff() {
  screen->TurnOff();
  set_rem = Setpoint;
  Setpoint = 0.0;
  analogWrite(FET_Pin, 0);
  PID_Ticks=0;
  state = OFF;
}

void strobe_WDT()
{
    WDT_State = !WDT_State;            // Toggle WDT output variable
    digitalWrite(WDT , WDT_State);    //Write to strobe pin
}

void delay_WDT(int ms)   // use this instaed of delay function to prevent WDT time out
{
   int ticks;
   for (ticks = 0; ticks < ms; ticks++)
   {
      delay(1);
      strobe_WDT();
   }
}

void TestPIDTime(){
    unsigned long SPID;
    unsigned long EPID;
    unsigned long ElapsedPID;

    while (true){
        Run_PID();
        read_temp();
        screen->UpdateTemperature(last_temperature, Temperature);
        last_temperature = Temperature;
    }
}

void loop() {
  switch (state) {
    case OFF:
      WaitToBeTurnedOn(); break;
    case ON:
      TurnOn(); break;
    case TEST:
      RunTests(); break;
    case RUN:
      Run(); break;
    default:
      break;
  }
}

void WaitToBeTurnedOn() {

  // wait for power button to be a non-depressed state
  while (PowerButtonIsDepressed()) { }

  int LedOutput = 0;
  bool readyToTurnOn = false;
  
  while (!PowerButtonIsDepressed()) {

    analogWrite(LEDA, LedOutput); // PWM the A side
    
    if (LedOutput == 255) x = -1;
    if (LedOutput == 0) x = 1;
    LedOutput += x;
    
    delay_WDT(10);

    while (PowerButtonIsDepressed()) { readyToTurnOn = true; }
    if (readyToTurnOn) { break; }
  }

  state = ON;
}

void TurnOn() {
  digitalWrite(LEDA, HIGH);
  Setpoint = set_rem;
  refresh = 1;
  screen->ShowArcLogo(&delay_WDT);
  state = TEST;
}

void RunTests() {
  if (PushButtonOneIsDepressed() && PushButtonThreeIsDepressed())
  {
    ALI_Tests();
  } else {
    
    read_temp();
    for (DT_index = 0; DT_index <= DT_SIZE; DT_index++)   // preload delta T (DT) array with current temperature
    {                                                   // this is used for calculating delta temperatures
      DT[DT_index] = Temperature;
      strobe_WDT();
    }
  
    if (SKIP_TEST_MODE_IS_ON){
        Serial.println("skip tests");
        state = RUN;
        return;
    }

    screen->PrintTftDataToSerial();
    Serial.println();
    Serial.println("*** Test RTD ***");
    RTD_Fault = 0;
    HTR_Fault = 0;
    read_temp_verbose();   // This will set RTD_Fault if there is a problem
    Serial.println();

    Serial.println("*** Heater Power Up Test ***");
    Heater_PU_Test();     // This will set HTR_Fault if there is a problem
    if ((RTD_Fault) || (HTR_Fault)) {
        screen->ShowPolyarcNotFound();
        wait_4_key_press();
        return;
    }
  }
}

void Run() {
  
  delay_WDT(1000);
  analogWrite(FET_Pin, 0);
  strobe_WDT();
  screen->ShowTestingHeader();

ST2:
    RTD_Fault = 0;
    HTR_Fault = 0;
    read_temp_verbose();   // This will set RTD_Fault if there is a problem
    Heater_PU_Test();     // This will set HTR_Fault if there is a problem
    
    if (!SKIP_TEST_MODE_IS_ON && ((RTD_Fault) || (HTR_Fault))) {
        screen->ShowPolyarcNotFound();
        wait_4_key_press();
        goto ST2;
    }
    
    Analog_Tests();           // Run tests of Voltage and Heater Current

    Recall_NVM();
    screen->ShowUseLastSetpointQuestion(Setpoint);
    wait_4_key_press();
    fault = 0;
    if (KEY1)
        goto SU1;
    if (KEY2)
        set_Setpoint();
    if (KEY3)
        goto ST2;              // Allows editting of SetPt, Kp, Ki, Kd
    if (fault != 0)         // check and see if we had a fault while editting setpoint or parameters
    {
        Break_Code = 5;
        goto FLT_Bail;
    }

SU1:
  Write_NVM();
  read_temp();             // Read the RTD for initial display of it

SU2:

  PID_Ticks = 0;
  state = RUN;
  
  screen->ShowMain(Setpoint, Temperature);
  
  while (true) {

    last_temperature = Temperature;
    last_setpoint = Setpoint;
  
    int down = 0;
    int held = 0;
    double multi = 1;
    
    if (PowerButtonIsDepressed()){     // Test for On/Off button press
        while (PowerButtonIsDepressed()) { }
        TurnOff();
        break;
    }

    while (PushButtonOneIsDepressed() && PushButtonTwoIsDepressed()) {
      if (held > 3000) {
        state = EDIT;
        analogWrite(FET_Pin,0); 
        Edit_Parameters();
        refresh = 1;
        PID_Ticks = 0;
        state = RUN;
      } else {
        held += 50;
        delay_WDT(50);
      }
      continue;
    }
  
    while (PushButtonOneIsDepressed() and set_on) {
      if (Setpoint <= 525) {
        analogWrite(FET_Pin,0);
        last_setpoint = Setpoint;
        Setpoint += (0.1) * multi;
        screen->UpdateSetpoint(last_setpoint, Setpoint);
        Write_NVM();
        if (down++ < 9) {
          delay_WDT(100);
        } else if (down++ < 18) {
          multi = 10;
          delay_WDT (100);
        } else {
          multi = 50;
          delay_WDT(100);
        }
      }
    }
    while(PushButtonTwoIsDepressed() && set_on){
      if (Setpoint >= 0) {
        analogWrite(FET_Pin,0);
        last_setpoint = Setpoint;
        Setpoint -= (0.1) * multi;
        screen->UpdateSetpoint(last_setpoint, Setpoint);
        Write_NVM();
        if (down++ < 10) {
          delay_WDT(100);
        } else if (down++<20) {
          multi = 10;
          delay_WDT (100);
        } else {
          multi = 50;
          delay_WDT(100);
        }
      }
    }
    if (PushButtonThreeIsDepressed()) {
      if (set_on) {
        Serial.println("**** User Hit Off Key ****");
        analogWrite(FET_Pin,0); 
        set_rem = Setpoint;
        last_setpoint = 0;
        Setpoint = 0;
        set_on = 0;
        screen->Resume();
        while (PushButtonThreeIsDepressed()){ }
        delay_WDT(333);
        continue;
      } else {
        analogWrite(FET_Pin,0); 
        Setpoint = set_rem;
        last_setpoint = Setpoint;
        set_on = 1;
        screen->Pause();
        while (PushButtonThreeIsDepressed){ }
        delay_WDT(333);
        continue;
      }
    }

    if (fault != 0) {         // check and see if we had a fault while editting setpoint or parameters
        Break_Code = 7;
        break;
    }

    strobe_WDT();
   
    if (RTD_Fault != 0) { 
      Break_Code = 2;
      break;
    }
 
    Run_PID(); 
    Int_Out = Output;
    analogWrite(FET_Pin, Int_Out);             // Write PID output to heater     

    Test_Heater();                            // Check and see if heater current is in tolerance
    if (HTR_Fault != 0) {
      Break_Code = 3;
      break;
    }

    if (PID_Ticks == 1) { // check heater every other cycle
      Current = analogRead(0); 
      if (Current < 500) { // check for over current (value gets lower as current increases
            Break_Code = 3;
            HTR_Fault = 1;    
      }
      if (Int_Out > 100) { // test for under current (only if we are driving heater at a reasonable level)
        if (Current > 600) { // 650-635 is zero current, 600 approx PWM 75, at PWM 100 we should see approx 580     
          Break_Code = 3;
          HTR_Fault = 2;
        } 
      }         
    }

    if (PID_Ticks == 2) {
      analogWrite(FET_Pin,0);  // turn off PWM and heater
      strobe_WDT();
      delay(5);
      read_temp();     // read the temp
      
      if(RTD_Fault == 4) { // Look for RTD noise fault
        Serial.println("****");
        Serial.println("Warning ... RTD Fault = 4 .. Under/Over voltage on inputs");
        RTD_Fault = 0;  // reset fault  
        Temperature = last_temperature;  // use the last good temperature
        Fault_Count++;
        Serial.print("FC = ");
        Serial.println(Fault_Count);
        Serial.print("Time = ");
        Serial.println(now / 1000, 1);
        Serial.println(); 
      }
             
      if ((Temperature != last_temperature) || (refresh == 1)) { // Update TFT only if data has changed to reduce flicker
        
        screen->UpdateTemperature(last_temperature, Temperature);
        last_temperature = Temperature;
        
        if (Output > 0) {
          screen->AddTemperatureStar();
        } else {
          screen->RemoveTemperatureStar();
        }
      }
    
      if (Setpoint != last_setpoint) {      // Update TFT only if data has changed to reduce flicker
        analogWrite(FET_Pin,0); 
        screen->UpdateSetpoint(last_setpoint, Setpoint);
        last_setpoint = Setpoint;
      }
      PID_Ticks = 0;
      refresh = 0;
      analogWrite(FET_Pin,Int_Out);
     }
  }

// To get here, we bailed from main loop above because PB was pressed or we had a fault

FLT_Bail:

  analogWrite(FET_Pin, 0);        // Turn Off Heater

  Serial.print("Break Code = ");
  Serial.println(Break_Code);

  switch (Break_Code) {
    case 1:
      Serial.println("PB Pressed .... Shutdown");
      break;
    case 2:
      Serial.print("RTD Fault ..."); Serial.println(RTD_Fault);
      break;
    case 3:
      Serial.print("Heater Fault = "); Serial.println(HTR_Fault);
      break;
    default:
      break;
  }
  Setpoint = 0;
}
