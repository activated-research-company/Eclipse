// ************ Reads the MAX RTC Chip and loads variable Temperature **************

void read_temp(bool filter) {

  max.readRTD();

  if (filter) {
    if (TemperatureFilter.Current() < 1) {
      TemperatureFilter.SetCurrent(max.temperature(100, RREF));
    } else {
      TemperatureFilter.Filter(max.temperature(100, RREF));
    }
    Temperature = TemperatureFilter.Current();
  } else {
    Temperature =  max.temperature(100, RREF);
  }
  
  uint8_t fault = max.readFault();
  RTD_Fault = fault;

  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    max.clearFault();
  }
}

void read_temp_verbose() {
  uint16_t rtd = max.readRTD();
  Serial.print("RTD value: ");
  Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;    // same as ratio = ratio / 32768;
  Serial.print("Ratio = ");
  Serial.println(ratio, 8);
  Serial.print("Resistance = ");
  Serial.println(RREF * ratio, 8);
  Temperature = max.temperature(100, RREF);
  Serial.print("Temperature = ");
  Serial.println(Temperature);
  //Check and print any faults
  uint8_t fault = max.readFault();
  Serial.print("Fault = ");
  Serial.println(fault);
  RTD_Fault = fault;

  if (fault) {
    Serial.print("Fault 0x");
    Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    max.clearFault();
  }
}
