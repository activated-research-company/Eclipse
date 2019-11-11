#include <Adafruit_MAX31865.h>
#include <EEPROM.h>
#include "Screen.h"
#include "SetpointController.h"
#include "WatchdogTimer.h"
#include <PID_v1.h>

//#define DEBUG_MODE // uncomment to turn on debug mode
//#define SKIP_TEST_MODE // uncomment to turn off tests

Adafruit_MAX31865 max = Adafruit_MAX31865(8, 7, 6, 5); // defines pins used for SPI: CS, DI, DO, CLK

#define RREF 430.0 // Rref value used with MAX31865 Chip

#define AUTOMATIC 1 // pid initialization
#define DIRECT 0 // pid initialization

#define Max_Htr 200 // Limits the Heater PWM to this value to prevent over drive (maximum 255 = 90W)

#define FET_Pin  13 // Heater drive PWM pin using power FET
#define LEDA 46 // PWM-able pin used for the power button ... 

unsigned long now;

double Output = 0;
double Setpoint;

double kp = 10.004;
double ki = 0.228;
double kd = 38.158;

int pidInterval = 500; // ms
unsigned long lastPidMillis = 0;

double Temperature;
double LastTemperature;

int RTD_Fault; // Gets set by RTD read function
int HTR_Fault; // Gets set by Test_Heater function

int Int_Out; // used for casting float Outputs as integer
int Fault_Count = 0;

int Power_Avg; // rolling average of power (drive) to FET

int fault; // This group used for analog tests
float Volts;
float Current;
float Adj_Current;
float Midpt;
float Resistance;
float Power;

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
PID pid(&Temperature, &Output, &Setpoint, kp, ki, kd, DIRECT);

void setup() {
  
  #ifdef DEBUG_MODE
    Serial.begin(9600);
  #endif
  
  screen = new Screen(10, 9);
  setpointController = new SetpointController(&Setpoint, 1, 0, 650, FET_Pin, screen);
  watchdogTimer = new WatchdogTimer(45);
  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(pidInterval);
  max.begin(MAX31865_2WIRE); // RTD code and RTD Type
  pinMode(LEDA, OUTPUT);
  pinMode(FET_Pin, OUTPUT);
  analogReference(INTERNAL2V56); // use 2.56 volts as reference
  analogWrite(FET_Pin, 0);
  state = OFF;
}

void TurnOff() {
  screen->TurnOff();
  analogWrite(FET_Pin, 0);
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
  watchdogTimer->Delay(2500);
  screen->ShowSoftwareVersion();
  watchdogTimer->Delay(2500);
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
    read_temp();   // This will set RTD_Fault if there is a problem

    Heater_PU_Test(); // This will set HTR_Fault if there is a problem
    if ((RTD_Fault) || (HTR_Fault)) {
        screen->ShowPolyarcNotFound();
        GetNextButtonPress(3, NULL);
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
  if (GetNextButtonPress(2, NULL) == PushButtonTwo) { set_Setpoint(); }
  if (fault != 0) { state = TEST; return;}
  if (state == OFF) { return; }

  read_temp();
  
  screen->ShowMain(Setpoint, Temperature);
  LastTemperature = Temperature;
  
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
      ShowDiagnostics();
      if (state == OFF) { return; }
      screen->ShowMain(Setpoint, Temperature);
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
    analogWrite(FET_Pin, 0);
    screen->Pause();
    screen->UpdateOutput(0);
    GetNextButtonPress(3, &ReadAndUpdateTemperature);
    if (state == OFF) { return; }
    if (state != OFF) {
      screen->Resume();
      screen->UpdateOutput(Output / 2.55);
    }
  }

  if (fault != 0) { // check and see if we had a fault while editting setpoint or parameters
      state = TEST;
      return;
  }

  watchdogTimer->Refresh();
 
  if (RTD_Fault != 0) { 
    state = TEST;
    return;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - lastPidMillis >= pidInterval) {

    lastPidMillis = currentMillis;
    
    analogWrite(FET_Pin, 0);
    read_temp();
    watchdogTimer->Refresh();

    pid.Compute();

    Int_Out = Output;
    analogWrite(FET_Pin, Int_Out);

    watchdogTimer->Refresh();
    screen->UpdateOutput(Output / 2.55);
    watchdogTimer->Refresh();
      
    #ifdef DEBUG_MODE
      Serial.println(Int_Out);
    #endif
    
    if(RTD_Fault == 4) { // Look for RTD noise fault
      RTD_Fault = 0;  // reset fault  
      Temperature = LastTemperature;  // use the last good temperature
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

    UpdateTemperature(); 
  }
}
