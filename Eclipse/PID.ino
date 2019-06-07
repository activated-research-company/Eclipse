void Run_PID()  // takes about 60 mS to run
{
  long Tin;

  now = millis();
  
  strobe_WDT(); 
  timeChange = (now - lastTime);
  if (timeChange >= SampleTime)           // return if not enough time has elapsed (Sample time is a define up front)
  {
    strobe_WDT(); 
    PID_Ticks++;                          // increments on 500mS tick
    print_datalog_data();                 // select either of these two for output. 
    
    Unfiltered_Temp = Temperature;        // read_Temp updates Temperature variable
    Tin = int(Unfiltered_Temp*10);        // run smoothing on raw temp (Integer math, so convert to int and scale up)
    Filtered_Temp = Smooth_Temp(Tin);
    Filtered_Temp = Filtered_Temp/10;

    Delta_T = Calc_Delta_T(Unfiltered_Temp);

    if (InFilt_State == true)
    {
        Temperature = Filtered_Temp;
        Input = Filtered_Temp;
    }
    else
    {
        Temperature = Unfiltered_Temp;
        Input = Unfiltered_Temp;
    }
    
    error = Setpoint - Input;  //Compute all the working error variables
    
    if (error >  20)    // Use straight prop control until we get close
    {
        PTerm = 15 * error;  // hardwire kp at 15 and hold I in reset
        ITerm = 0;
        DTerm = 0;
    }
    else
    {
        ITerm += ((ki / 100) * error);
        if (ITerm > outMax) ITerm = outMax;
        else if (ITerm < outMin) ITerm = outMin;
    
        double dInput = (Input - lastInput);
        DTerm = kd * dInput;
        PTerm = kp * error;
    }
    
    Output = PTerm + ITerm - DTerm;               //Compute PID Output by summing P,I,D terms
    double T = Output;                            // save unclipped Output
    if (Output > Max_Htr) Output = Max_Htr;       // Clip output to be within min and max
    else if (Output < outMin) Output = outMin;

    Power_Avg = Calc_Power_Avg(Output);                       // Calculate average power (over 30 seconds)

    Unfiltered_Out = Output;                    // Now smooth the output
    Tin = int(Unfiltered_Out*10);
    Filtered_Out = Smooth_Output(Tin);
    Filtered_Out = Filtered_Out/10;

    if (OutFilt_State == true)
    {
        Output = Filtered_Out;
    }
    else
    {
        Output = Unfiltered_Out;
    }
    lastInput = Input;  //Remember some variables for next time
    lastTime = now; 
  }
  
  strobe_WDT();
}

long Smooth_Temp(int M) {
  ST_sum -= ST[ST_index];
  ST[ST_index] = M;
  ST_sum += ST[ST_index];
  ST_index++;
  ST_index = ST_index % ST_SIZE;
  if (ST_count < ST_SIZE) ST_count++;
  return ST_sum / ST_count;
}

long Smooth_Output(int M) {
  SO_sum -= SO[SO_index];
  SO[SO_index] = M;
  SO_sum += SO[SO_index];
  SO_index++;
  SO_index = SO_index % SO_SIZE;
  if (SO_count < SO_SIZE) SO_count++;
  return SO_sum / SO_count;
}

long Calc_Power_Avg(int M) {
  PA_sum -= PA[PA_index];
  PA[PA_index] = M;
  PA_sum += PA[PA_index];
  PA_index++;
  PA_index = PA_index % PA_SIZE;
  if (PA_count < PA_SIZE) PA_count++;
  return PA_sum / PA_count;
}

long Calc_Current_Avg(int M) {
  CA_sum -= CA[CA_index];
  CA[CA_index] = M;
  CA_sum += CA[CA_index];
  CA_index++;
  CA_index = CA_index % CA_SIZE;
  if (CA_count < CA_SIZE) CA_count++;
  return CA_sum / CA_count;
}

long Calc_Delta_T(int M) {
  int delta_temp;
  delta_temp= M-DT[DT_index];
  DT[DT_index] = M;
  DT_index++;
  DT_index = DT_index % DT_SIZE;  // modulo operator rolls index to 1 if we exceed size
  if (DT_count < DT_SIZE) DT_count++;
  return delta_temp;
}

void SetTunings(double Kp, double Ki, double Kd)
{
  if (Kp < 0 || Ki < 0 || Kd < 0) return;

  double SampleTimeInSec = ((double)SampleTime) / 1000;
  kp = Kp;
  ki = Ki * SampleTimeInSec;
  kd = Kd / SampleTimeInSec;

  if (controllerDirection == REVERSE)
  {
    kp = (0 - kp);
    ki = (0 - ki);
    kd = (0 - kd);
  }
}
void SetSampleTime(int NewSampleTime)
{
  if (NewSampleTime > 0)
  {
    double ratio  = (double)NewSampleTime
                    / (double)SampleTime;
    ki *= ratio;
    kd /= ratio;
    SampleTime = (unsigned long)NewSampleTime;
  }
}

void SetOutputLimits(double Min, double Max)
{
  if (Min > Max) return;
  outMin = Min;
  outMax = Max;

  if (Output > outMax) Output = outMax;
  else if (Output < outMin) Output = outMin;

  if (ITerm > outMax) ITerm = outMax;
  else if (ITerm < outMin) ITerm = outMin;
}

void SetMode(int Mode)
{
  bool newAuto = (Mode == AUTOMATIC);
  if (newAuto == !inAuto)
  { /*we just went from manual to auto*/
    InitializePID();
  }
  inAuto = newAuto;
}

void InitializePID()
{
  lastInput = Input;
  ITerm = Output;
  if (ITerm > outMax) ITerm = outMax;
  else if (ITerm < outMin) ITerm = outMin;
}

void SetControllerDirection(int Direction)
{
  controllerDirection = Direction;
}
