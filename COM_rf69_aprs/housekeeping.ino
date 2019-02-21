double hk_get_internal_temperature(void)
{
  unsigned int wADC;
  double t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - INTERNALCALIB ) / 1.22;

  // The returned temperature is in degrees Celsius.
  return (t);
}


void hk_get_housekeeping()
{
//  int radio_temp=0;
//  int radio_temp_avg=0;
  int temp_send=0;
  double int_temp_tmp = 0.0;
  
  temp_avg = 0.0;
  int_temp_avg = 0.0;
  
  wdt_reset();
  for (int i=0; i<AVRG; i++)
  {
    delay(10);
    int_temp_tmp = hk_get_internal_temperature();
    int_temp_tmp = (int_temp_tmp + CALIBRATION_OFFSET) * CALIBRATION_GAIN;
    int_temp_avg += int_temp_tmp; 
//    radio_temp_avg += radio.readTemperature(1);
  }
  
  wdt_reset();
//  radio_temp = radio_temp_avg/AVRG;
  int_temp = int_temp_avg/((double)AVRG);
   
//  temp_send= (((int)(int_temp*10)) + (radio_temp*10))/2;
  temp_send= (int)(int_temp*10);

  sprintf(COM_temp, "%04d", temp_send);

#ifdef debug2
  Serial.print(F("COM: HouseKeeping: COM Temperature: ")); //debug
  Serial.println(COM_temp);  
#endif 
}
