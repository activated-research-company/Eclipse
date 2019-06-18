#include <Adafruit_MAX31865.h>
#include <EEPROM.h>
#include "Screen.h"
#include "SetpointController.h"
#include "WatchdogTimer.h"

//#define DEBUG_MODE // uncomment to turn on debug mode
//#define SKIP_TEST_MODE // uncomment to turn off tests

Adafruit_MAX31865 max = Adafruit_MAX31865(8, 7, 6, 5); // defines pins used for SPI: CS, DI, DO, CLK

#define RREF 430.0   // Rref value used with MAX31865 Chip

#define MANUAL 0      // For PID Loop 
#define AUTOMATIC 1   // For PID Loop

#define DIRECT 0      // For PID Loop
#define REVERSE 1     // For PID Loop

#define Max_Htr 200  // Limits the Heater PWM to this value to prevent over drive (maximum 255 = 90W)

//***** Hardware Pins Defined Here *****
#define FET_Pin  13   // Heater drive PWM using power FET

#define LEDA 46     // PWM-able and used for the power button ... 

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

#define PowerButton 0
#define PushButtonOne 1
#define PushButtonTwo 2
#define PushButtonThree 4

int state;
bool goToNextState;
#define OFF 0
#define ON 1
#define TEST 2
#define EDIT 3
#define RUN 4

Screen* screen;
SetpointController* setpointController;
WatchdogTimer* watchdogTimer;

void setup() {
  screen = new Screen(10, 9);
  setpointController = new SetpointController(&Setpoint, 0.1, 0, 525, FET_Pin, screen);
  watchdogTimer = new WatchdogTimer(45);
  Serial.begin(9600);
  max.begin(MAX31865_2WIRE); // RTD code and RTD Type
  SetControllerDirection(DIRECT);
  SetSampleTime(500); // loop rate (500 = 2 Hz)
  SetOutputLimits(0, 255); // min and max for output
  SetMode(AUTOMATIC);
  SetTunings(2, .5, 0); // initial tuning parameters - P, I, D
  pinMode(LEDA, OUTPUT);
  pinMode(FET_Pin, OUTPUT);
  InFilt_State = 0;
  OutFilt_State = 0; // int filter states, NVM recall will override later
  analogReference(INTERNAL2V56); // use 2.56 volts as reference
  analogWrite(FET_Pin, 0);
  state = OFF;
}

void TurnOff() {
  screen->TurnOff();
  analogWrite(FET_Pin, 0);
  PID_Ticks=0;  
  state = OFF;
}

void loop() {
  if (goToNextState) { state = state + 1 % 5; goToNextState = false; }

  switch (state) {
    case OFF:
      WaitToBeTurnedOn(); break;
    case ON:
      TurnOn(); break;
    case TEST:
      RunTests(); break;
    case EDIT:
      Edit(); break;
    case RUN:
      Run(); break;
    default:
      break;
  }
}

void WaitToBeTurnedOn() {

  // wait for power button to be a non-depressed state
  while (PowerButtonIsDepressed()) { watchdogTimer->Refresh(); }

  int ledOutput = 0;
  int ledOutputDirection;
  bool readyToTurnOn = false;
  
  while (PowerButtonIsNotDepressed()) {

    analogWrite(LEDA, ledOutput);
    
    if (ledOutput == 255) ledOutputDirection = -1;
    if (ledOutput == 0) ledOutputDirection = 1;
    ledOutput += ledOutputDirection;
    
    watchdogTimer->Delay(10);

    while (PowerButtonIsDepressed()) { readyToTurnOn = true; watchdogTimer->Refresh(); }
    if (readyToTurnOn) { break; }
  }

  goToNextState = true;
}

void TurnOn() {
  digitalWrite(LEDA, LOW);
  screen->ShowArcLogo();
  watchdogTimer->Delay(2000);
  screen->ShowSoftwareVersion();
  watchdogTimer->Delay(2000);
  digitalWrite(LEDA, HIGH);
  goToNextState = true;
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
      watchdogTimer->Refresh();
    }
  
    #ifdef SKIP_TEST_MODE
        Serial.println("skipped tests");
        goToNextState = true;
        return;
    #endif

    #ifdef DEBUG_MODE
      screen->PrintTftDataToSerial();
    #endif

    RTD_Fault = 0;
    HTR_Fault = 0;
    read_temp_verbose();   // This will set RTD_Fault if there is a problem

    Heater_PU_Test(); // This will set HTR_Fault if there is a problem
    if ((RTD_Fault) || (HTR_Fault)) {
        screen->ShowPolyarcNotFound();
        GetNextButtonPress(3, &Run_PID);
        return;
    }
    
    Analog_Tests(); // Run tests of Voltage and Heater Current
  }

  goToNextState = true;
}

void Edit() {
  Recall_NVM();
  screen->ShowUseLastSetpointQuestion(Setpoint);
  fault = 0;
  if (GetNextButtonPress(2, &Run_PID) == PushButtonTwo) { set_Setpoint(); }
  if (fault != 0) { state = TEST; return;}
  if (state == OFF) { return; }

  read_temp();

  PID_Ticks = 0;
  
  screen->ShowMain(Setpoint, Temperature);
  last_temperature = Temperature;
  
  goToNextState = true;
}

void Run() {
        
  if (PowerButtonIsDepressed()) {
      while (PowerButtonIsDepressed()) { }
      TurnOff();
      return;
  }

  int held = 0;

  while (PushButtonOneIsDepressed() && PushButtonTwoIsDepressed()) {
    if (held > 3000) {
      analogWrite(FET_Pin,0); 
      Edit_Parameters();
      if (state == OFF) { return; }
      screen->ShowMain(Setpoint, Temperature);
      PID_Ticks = 0;
    } else {
      held += 50;
      watchdogTimer->Delay(50);
    }
  }

  while (PushButtonOneIsDepressed()) {
    setpointController->IncrementSetpoint(watchdogTimer, &PushButtonOneIsNotDepressed);
  }
  
  while (PushButtonTwoIsDepressed()) {
    setpointController->DecrementSetpoint(watchdogTimer, &PushButtonTwoIsNotDepressed);
  }
  
  if (PushButtonThreeIsDepressed()) {
    analogWrite(FET_Pin,0);
    screen->Pause();
    screen->RemoveTemperatureStar();
    GetNextButtonPress(3, &ReadAndUpdateTemperature);
    if (state == OFF) { return; }
    if (state != OFF) { screen->Resume(); }
  }

  if (fault != 0) { // check and see if we had a fault while editting setpoint or parameters
      state = TEST;
      return;
  }

  watchdogTimer->Refresh();
 
  if (RTD_Fault != 0) { 
    Break_Code = 2;
    state = TEST;
    return;
  }

  Run_PID(); 
  Int_Out = Output;
  analogWrite(FET_Pin, Int_Out);             // Write PID output to heater     

  if (PID_Ticks == 2) {
    analogWrite(FET_Pin, 0);  // turn off PWM and heater
    watchdogTimer->Refresh();
    read_temp();     // read the temp
    
    if(RTD_Fault == 4) { // Look for RTD noise fault
      RTD_Fault = 0;  // reset fault  
      Temperature = last_temperature;  // use the last good temperature
      Fault_Count++;
      #ifdef DEBUG_MODE
        Serial.println("****");
        Serial.println("Warning ... RTD Fault = 4 .. Under/Over voltage on inputs");
        Serial.print("FC = ");
        Serial.println(Fault_Count);
        Serial.print("Time = ");
        Serial.println(now / 1000, 1);
        Serial.println(); 
      #endif
    }
           
    UpdateTemperature(true);
  
    PID_Ticks = 0;
    analogWrite(FET_Pin, Int_Out);
   }
}
