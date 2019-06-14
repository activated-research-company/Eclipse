#include <Adafruit_MAX31865.h>
#include <EEPROM.h>
#include "Screen.h"
#include "SetpointController.h"

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

/*** Variables used in PID Loop ***/
unsigned long lastTime;   // used for integration time compute
unsigned long now;        // used for integration time compute
unsigned long timeChange;
int PID_Ticks = 0;            // increments on 500mS tick (if PID loop is 500mS)

double Input = 0;
double Output = 0;
double Setpoint;
double PTerm, ITerm, DTerm, lastInput;
double kp = 50;
double ki = 13;
double kd = 255;
double error;
double Temperature;
double last_temperature;
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

int fault;        // This group used for analog tests
float Volts;
float Current;
float Adj_Current;
float Midpt;
float Av;
float Resistance;
float Power;

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

#define PowerButton 0
#define PushButtonOne 1
#define PushButtonTwo 2
#define PushButtonThree 4

int state;
#define OFF 0
#define ON 5
#define TEST 4
#define RUN 1
#define EDIT 2
#define SLEEP 3

Screen* screen;
SetpointController* setpointController;

void setup() {
  screen = new Screen(10, 9);
  setpointController = new SetpointController(&Setpoint, 0.1, 0, 525, FET_Pin, screen);
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

void delay_WDT(int ms) { // use this instaed of delay function to prevent WDT time out
   for (int ticks = 0; ticks < ms; ticks++) {
      delay(1);
      strobe_WDT();
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

  int ledOutput = 0;
  int ledOutputDirection;
  bool readyToTurnOn = false;
  
  while (!PowerButtonIsDepressed()) {

    analogWrite(LEDA, ledOutput);
    
    if (ledOutput == 255) ledOutputDirection = -1;
    if (ledOutput == 0) ledOutputDirection = 1;
    ledOutput += ledOutputDirection;
    
    delay_WDT(10);

    while (PowerButtonIsDepressed()) { readyToTurnOn = true; }
    if (readyToTurnOn) { break; }
  }

  state = ON;
}

void TurnOn() {
  digitalWrite(LEDA, HIGH);
  screen->ShowSplashScreen(&delay_WDT);
  state = TEST;
}

void RunTests() {

  screen->ShowTestingHeader();
  
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

    Serial.println("*** Test RTD ***");
    RTD_Fault = 0;
    HTR_Fault = 0;
    read_temp_verbose();   // This will set RTD_Fault if there is a problem

    Serial.println("*** Heater Power Up Test ***");
    Heater_PU_Test();     // This will set HTR_Fault if there is a problem
    if ((RTD_Fault) || (HTR_Fault)) {
        screen->ShowPolyarcNotFound();
        GetNextButtonPress();
        return;
    }
    
    Analog_Tests(); // Run tests of Voltage and Heater Current
  }

  state = RUN;
}

void Run() {
  
    Recall_NVM();
    
    screen->ShowUseLastSetpointQuestion(Setpoint);
    fault = 0;
    switch (GetNextButtonPress()) {
      case PushButtonTwo: set_Setpoint(); break;
    }
    if (fault != 0)         // check and see if we had a fault while editting setpoint or parameters
    {
        Break_Code = 5;
        goto FLT_Bail;
    }

  read_temp();             // Read the RTD for initial display of it

SU2:

  PID_Ticks = 0;
  state = RUN;
  
  screen->ShowMain(Setpoint, Temperature);
  last_temperature = Temperature;
  
  while (true) {
  
    double multi = 1;
    
    if (PowerButtonIsDepressed()) {
        while (PowerButtonIsDepressed()) { }
        TurnOff();
        break;
    }

    int held = 0;

    while (PushButtonOneIsDepressed() && PushButtonTwoIsDepressed()) {
      if (held > 3000) {
        state = EDIT;
        analogWrite(FET_Pin,0); 
        Edit_Parameters();
        PID_Ticks = 0;
        state = RUN;
      } else {
        held += 50;
        delay_WDT(50);
      }
      continue;
    }
  
    while (PushButtonOneIsDepressed()) {
      setpointController->IncrementSetpoint(&delay_WDT, &PushButtonOneIsNotDepressed);
    }
    while (PushButtonTwoIsDepressed()) {
      setpointController->DecrementSetpoint(&delay_WDT, &PushButtonTwoIsNotDepressed);
    }
    
    if (PushButtonThreeIsDepressed()) {
      analogWrite(FET_Pin,0);
      screen->Pause();
      while (PushButtonThreeIsDepressed()) { strobe_WDT(); }
      while (PushButtonThreeIsNotDepressed()) { strobe_WDT(); }
      while (PushButtonThreeIsDepressed()) { strobe_WDT(); }
      screen->Resume();
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
             
      if ((Temperature != last_temperature)) { // update TFT only if data has changed to reduce flicker
        
        screen->UpdateTemperature(last_temperature, Temperature);
        last_temperature = Temperature;
        
        if (Output > 0) {
          screen->AddTemperatureStar();
        } else {
          screen->RemoveTemperatureStar();
        }
      }
    
      PID_Ticks = 0;
      analogWrite(FET_Pin, Int_Out);
     }
  }

FLT_Bail: // power button or fault

  analogWrite(FET_Pin, 0);        // Turn Off Heater

  Serial.print("Break Code = ");
  Serial.println(Break_Code);

  switch (Break_Code) {
    case 1:
      Serial.println("PB Pressed .... Shutdown");
      break;
    case 2:
      Serial.print("RTD Fault ...");
      Serial.println(RTD_Fault);
      break;
    case 3:
      Serial.print("Heater Fault = ");
      Serial.println(HTR_Fault);
      break;
    default:
      break;
  }
}
