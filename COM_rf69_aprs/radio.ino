void radio_init(void)
{
  radio.initialize(FREQUENCY,NODEID,NETWORKID);

  //set frequency 432.966Mhz
#ifdef COM_RF69HCW1   
  radio.writeReg(0x07, 0x6C); //REG_FRFMSB
  radio.writeReg(0x08, 0x42); //REG_FRFMID
  radio.writeReg(0x09, 0xCC); //REG_FRFLSB
#elif defined(COM_RF69HCW2) 
  radio.writeReg(0x07, 0x6C); //REG_FRFMSB
  radio.writeReg(0x08, 0x42); //REG_FRFMID  
  radio.writeReg(0x09, 0xC0); //REG_FRFLSB  
#elif defined(COM_RF69CW1) 
  radio.writeReg(0x07, 0x6C); //REG_FRFMSB
  radio.writeReg(0x08, 0x42); //REG_FRFMID  
  radio.writeReg(0x09, 0xCC); //REG_FRFLSB  
#elif defined(COM_RF69CW2) 
  radio.writeReg(0x07, 0x6C); //REG_FRFMSB
  radio.writeReg(0x08, 0x42); //REG_FRFMID  
  radio.writeReg(0x09, 0xBE); //REG_FRFLSB  
#endif
  //set bitrate 1kbsp
  radio.writeReg(0x03, 0xC8); //REG_BITRATEMSB
  radio.writeReg(0x04, 0x00); //REG_BITRATELSB 

  //set deviation 2kHz
  radio.writeReg(0x05, 0x00); //REG_FDEVMSB
  radio.writeReg(0x06, 0x20); //REG_FDEVLSB    
  
  //set receiver treshold
  radio.writeReg(0x29, 0xFF);

  //disable OCP
  radio.writeReg(0x13, 0x0F);

  //set maximum output 0x11
  radio.writeReg(0x11, 0x7F);


//  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
//
//#ifdef COM_RF69HCW1   
//  radio.setHighPower(true);    // Only for RFM69HCW & HW!
//  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
//#elif defined(COM_RF69HCW2) 
//  radio.setHighPower(true);    // Only for RFM69HCW & HW!
//  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
//#endif
////  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
////  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  
//  radio.encrypt(ENCRYPTKEY);  //if encryption used
  
  delay(500);
}


int radio_telemtery_tx()
{
  int i=0;
  int m_i=0;
  unsigned char* tmp;
  int ok=0;
  unsigned long timer=0;
  char mID[4];
  
  //copy telemetry data
  wdt_reset();
  
#ifdef debug1
  Serial.println(F("COM: Telemetry: Generate Radio Packet")); //debug
#endif 
  
  sprintf(mID, "%03d", (msg_id%1000));
  
  memcpy(telemetry +10, mID, 3);
  telemetry[13] = ',';
  memcpy(telemetry +14, GPS_time, 6);
  telemetry[20] = ',';
  memcpy(telemetry +21, GPS_lati, 9);
  telemetry[30] = ',';
  memcpy(telemetry +31, GPS_long, 10);
  telemetry[41] = ',';
  memcpy(telemetry +42, GPS_alti, 5);
  telemetry[47] = ',';
  memcpy(telemetry +48, EXT_temp, 4);
  telemetry[52] = ',';
  memcpy(telemetry +53, OBC_temp, 3);
  telemetry[56] = ',';
  memcpy(telemetry +57, COM_temp, 3);
  telemetry[60] = ',';
  
  /////////////////////////////
  //radio msg send
  /////////////////////////////
#ifdef debug1
  Serial.println(F("COM: Telemetry: Send Radio Packets")); //debug
#endif  

  wdt_reset();
  radio.send(RECEIVER, telemetry, 61, false);

#ifdef debug1
  Serial.println(F("COM: Telemetry: Radio Packet no.1 Sent")); //debug
#endif

  wdt_reset();
  delay(1000);
  
  wdt_reset();
  radio.send(RECEIVER, telemetry, 61, false);

#ifdef debug1
  Serial.println(F("COM: Telemetry: Radio Packet no.2 Sent")); //debug
#endif
  
  wdt_reset();
  radio.receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  msg_id++;
  return 0;
}

void radio_debug_msg_tx()
{
  wdt_reset();
  radio.send(RECEIVER, "##HAxUPRA,COMMAND ARRIVED------------------------------------" , 61, false);

#ifdef debug1
  Serial.println(F("COM: Response: Radio Packet no.1 Sent")); //debug
#endif
  
  wdt_reset();
  delay(1000);
  
  wdt_reset();
  radio.send(RECEIVER, "##HAxUPRA,COMMAND ARRIVED------------------------------------", 61, false);
  
#ifdef debug1
  Serial.println(F("COM: Response: Radio Packet no.2 Sent")); //debug
#endif
  
  wdt_reset();
  radio.receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU  
}

int radio_picture_data_tx(void) //TODO: in progress
{
  byte buf[61];
  static int i = 0;
  static int k = 0;
  uint8_t temp = 0,temp_last=0;
  bool is_data=false;
  volatile int cam_wtchdg=0;
  int nowtime=0; 
  
  
 i = 0;
 wdt_reset();
 cam_wtchdg = millis();
// while(!Serial.available())
// {
//    nowtime=millis();
//    if(nowtime - cam_wtchdg >3000)
//    {        
//      return 2;
//    }    
// }
/*  while (Serial.available() > 0)
  {
    temp = Serial.read();
  }*/
  temp=0; 
 //Read JPEG data from FIFO
 cam_wtchdg = millis();
 wdt_reset();
 while ( (temp !=0xD9) | (temp_last !=0xFF))
 {
  nowtime=millis();
  wdt_reset();
  if(nowtime - cam_wtchdg >5000)
  {        
    return 2;
  }  
  if (Serial.available() > 0)
  {
    cam_wtchdg = millis();
    temp_last = temp;
    temp = Serial.read();  //Write image data to buffer if not full

    if( i < 61)
    {
     buf[i++] = temp;
    }
    else
    {
      //Write 61 bytes image data to dataFile
      wdt_reset();
      radio.send(RECEIVER, buf, 61, false); 
      delay(100);       
      i = 0;

      //timeout in case of corrupt dadaflow; picture data larger than 80kB -> must be tested and calibrated
      if(k>320)
      {
        return 1;
      }
      
      buf[i++] = temp;
  
    }
    Serial.print(F("o")); 
    delay(0);  
 
  }
 }
 
 //Write the remain bytes in the buffer
 if(i > 0)
 {
    for( ; i < 61; i++)
    {
      buf[i] = 0;
    }
    wdt_reset();
    radio.send(RECEIVER, buf, 61, false);        
 }
 Serial.print(F("[COM] : PIC SEND END"));
 //Close the dataFile
  return 0;    
}

int32_t radio_receive_message(void)
{
    if( radio.SENDERID == RECEIVER)
    {
    #ifdef debug1
      Serial.println(F("COM: Incoming: Verified ID")); //debug
    #endif          
      last_msg_RSSI = radio.RSSI;
      //Serial.println((char*)radio.DATA);
      wdt_reset();
      for(int i=0; i<64; i++)
      {
        Serial.print((char)radio.DATA[i]);
      }
      Serial.println("");
      Serial.print("RSSI: ");
      Serial.print(last_msg_RSSI); 
      Serial.println(" dBm");  

      radio_debug_msg_tx();
    }
    else
    {
      Serial.println(F("COM: Incoming: Not Verified ID"));
      last_msg_RSSI = radio.RSSI;
      //Serial.println((char*)radio.DATA);
      wdt_reset();
      for(int i=0; i<64; i++)
      {
        Serial.print((char)radio.DATA[i]);
      }
      Serial.println("");
      Serial.print("RSSI: ");
      Serial.print(last_msg_RSSI); 
      Serial.println(" dBm");  
    }
}

