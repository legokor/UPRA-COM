void sicl_init(void)
{
  Serial.begin(  SICL_BAUD);

  pinMode(BUSBUSY, INPUT);

  
  // reserve 100 bytes for the inputString:
  inputString.reserve(100);
  isSerial_enabled = false;
 // BB_int_setup();
  
}

void BB_int_setup(void)
{
    *digitalPinToPCMSK(BUSBUSY) |= bit (digitalPinToPCMSKbit(BUSBUSY));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(BUSBUSY)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(BUSBUSY)); // enable interrupt for the group
}

ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
     isSerial_enabled = true;
//     Serial.println("bb interrupt");
}  

bool sicl_busBusy_interrupt(void)
{
  if (digitalRead(BUSBUSY) != 1)
  {
    return true;
  }
  return false;
}

void sicl_set_bus_busy(void)
{
  pinMode(BUSBUSY, OUTPUT);
  digitalWrite(BUSBUSY, LOW);
}

void sicl_clr_bus_busy(void)
{
  digitalWrite(BUSBUSY, HIGH);
  pinMode(BUSBUSY, INPUT);
}

/*
 * NMEA Parser
 */
int sicl_process_internal_msg()
{
  int i=0;
  int msg_index=0;
  int msg_code = 0;

  wdt_reset();
  for(i=0; i<100; i++)
  {
    if ((inputString[i] =='$') || (msg_index >= 80))
    {
      msg_index = 0;
    }
 
    if (inputString[i] != '\r')
    {
      msg[msg_index++] = inputString[i];
    }
 
    if (inputString[i] == '\n')
    {
      i = 200;
     // msg_index = 0;
    }    
  }
  if ((msg[1] == 'T') && (msg[2] == 'M') && (msg[3] == 'L') && (msg[4] == 'T') && (msg[5] == 'M')) /**< Telemetry TX packet */
  {
    wdt_reset();
  #ifdef debug1
    Serial.println(F("COM: BUS: Low-speed telemetry arrived")); //debug
  #endif
    processTMLTMcommand(msg_index);
    msg_index=0;
    msg_code = 1;
  }
  else if ((msg[1] == 'T') && (msg[2] == 'M') && (msg[3] == 'H') && (msg[4] == 'K') && (msg[5] == 'R')) /**< House Keeping Request packet */
  {
    wdt_reset();
  #ifdef debug1
    Serial.println(F("COM: BUS: HK request arrived")); //debug
  #endif
 
    processTMHKRcommand(msg_index);
    msg_index=0;
    msg_code = 2; 
  }
  else if ((msg[1] == 'P') && (msg[2] == 'C') && (msg[3] == 'S') && (msg[4] == 'C') && (msg[5] == 'S')) /**< Set Callsign packet */
  {
    wdt_reset();
  #ifdef debug1
    Serial.println(F("COM: BUS: Set Callsign request arrived")); //debug
  #endif

    processPCSCScommand(msg_index);
    msg_index=0;
    msg_code = 3;
  }  
  else if ((msg[3] == 'G') && (msg[4] == 'C') && (msg[5] == 'S')) /**< Get Callsign packet */
  {
    wdt_reset();
  #ifdef debug1
    Serial.println(F("COM: BUS: Read Callsign request arrived")); //debug
  #endif
    processXXGCScommand();
    msg_index=0;
    msg_code = 4;
  }    
  else if ((msg[1] == 'T') && (msg[2] == 'M') && (msg[3] == 'D') && (msg[4] == 'T') && (msg[5] == 'A')) /**< Large Data Transfer packet */
  {
    wdt_reset();
  #ifdef debug1
    Serial.println(F("COM: BUS: High Speed Data TX")); //debug
  #endif
 
    processTMDTAcommand(msg_index);
    msg_index=0;
    msg_code = 5; 
  }
  isSerial_enabled = false;
  //Serial.end();
  return msg_code;
}

void processXXGCScommand()
{
  char i;
  
  EEPROM_read_callsign();
  Serial.print("$TCSGN,");
  for( i=0; i<7; i++)
  {
    Serial.print(callsign[i]);
  }
  Serial.println("*55");
}

void processPCSCScommand(int msg_index)
{
  int i, j, k, IntegerPart;
   // $PCSCS,CALLSIGN,,,,*47
  //       COM 
  //        1  
 
  IntegerPart = 1;

 
  for (i=0, j=0, k=0; (i<msg_index) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if (msg[i] == ',')
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        if(msg[i] != '*')
        {
          new_callsign[k] = msg[i];
          k++;
        }
        else
        {
          j++;
        }
      }
    }
  }
  EEPROM_store_callsign(); 
  EEPROM_callsign_set();
  Serial.println("$TCACK,C,*47");
}

void processTMHKRcommand(int msg_index)
{
  int i, j, k, IntegerPart;
  char tmp[3];
 
  // $TMHKR,C,,,,*47
  //       COM 
  //        1  
 
  IntegerPart = 1;

 
  for (i=0, j=0, k=0; (i<msg_index) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if (msg[i] == ',')
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        tmp[k] = msg[i];
        k++;
      }

    }
    
   // GPS_Altitude = Altitude;
  }

  if( tmp[0] == 'C')
  {
    wdt_reset();
    delay(200);
    hk_get_housekeeping();
    Serial.print("$TCHKD,");
    Serial.print(COM_temp);
    Serial.print(",");
    Serial.print("336");
    Serial.print(",");
    Serial.print(msg_id);
    Serial.println("*47");
  #ifdef debug1
    Serial.println(F("COM: BUS: HK response sent")); //debug
  #endif    
  }
}

void processTMDTAcommand(int msg_index) //TODO : in progress
{
  int i, j, k, IntegerPart;
  char tmp[3];
 
  // $TMDTA,S,,,,*47
  //       Start 
  //        1  
 
  IntegerPart = 1;

 
  for (i=0, j=0, k=0; (i<msg_index) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if ((msg[i] == ',') || (msg[i] == '*'))
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        tmp[k] = msg[i];
        k++;
      }

    }
    
   // GPS_Altitude = Altitude;
  }

  if( tmp[0] == 'S')
  {
    wdt_reset();
    delay(200);
    hk_get_housekeeping();
    Serial.print(F("$TCACK,B"));
    Serial.println("*47");
  #ifdef debug1
    Serial.println(F("COM: BUS: HK response sent")); //debug
  #endif    
  }
}

void processTMLTMcommand(int msg_index)
{
  int i, j, k, IntegerPart;
  unsigned int Altitude;
  int time_msec = 0;

 
  // $TCLTM,123541,+4807.038,+01131.000,00545,0123,0123*47
  //         TIME     LAT        LONG    ALT   EXT  OBC                   
  //           1       2          3       4     5    6
 
  IntegerPart = 1;

 
  for (i=0, j=0, k=0; (i<msg_index) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if (msg[i] == ',')
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        if ((msg[i] >= '0') && (msg[i] <= '9'))
        {        
          GPS_time[k] = msg[i];
          k++;
        }
      }
      else if (j == 2)
      {
        GPS_lati[k] = msg[i];
        k++;
      }
      else if (j == 3)
      {
        GPS_long[k] = msg[i];
        k++;
      }
      else if (j == 4)
      {
        GPS_alti[k] = msg[i];
        k++;
      }
      else if (j == 5)
      {
        EXT_temp[k] = msg[i];
        k++;
      }
      else if (j == 6)
      {
        if(msg[i] == '*')
        {
          IntegerPart=0;
        }
        if(IntegerPart == 1)
        {
          OBC_temp[k] = msg[i];
          k++;
        }
      }
    }
    
   // GPS_Altitude = Altitude;
  }
  GPS_lati[11] ='\0';
  GPS_long[12] ='\0';
  GPS_time[7] ='\0';    
  GPS_alti[6] ='\0';  
  EXT_temp[5] ='\0';  
  OBC_temp[5] ='\0';  
}

void clear_buffer()
{
  int i=0;
  for(i=0; i<64; i++)
  {
    msg[i] = 0;
  }
}

