/***********************************************
 * COM RFM69HCW
 ***********************************************/

//#include <SoftwareSerial.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <EEPROM.h>
#include "eeprom.h"
#include "bus.h"
#include "radio.h"
#include "housekeeping.h"

//include headers for watchdog
/////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!! bootloader version: 2017_07_05 !!!!!!!!!!!!!!!!!!!!//
/////////////////////////////////////////////////////////////////////////
#include <avr/io.h> 
#include <avr/wdt.h>


void setup() {
  // put your setup code here, to run once:
  sicl_init();
  
  radio_init();  
  EEPROM_callsign_set();
  
#ifdef debug1
  Serial.print(F("COM: Callsign: "));
  Serial.println(callsign);  
#endif
  
  Serial.println(F("$TCSTR,1,0*47"));
  //Serial.println("$TCSTR,1,0*cc"); //Telecommand system start, UHF, APRS
  delay(1000);
  
  //enable watchdog: 8s
  wdt_enable(9);  
}

///////////////////////////////
//MAIN LOOP
///////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:
  wdt_reset();
  if(sicl_busBusy_interrupt())
  {
//    Serial.println(F("COM: BUS INTERRUPT"));
    isSerial_enabled = true;
//    Serial.begin(SICL_BAUD);
    //send ACK message
    //get BUS message
  }

  if (stringComplete) 
  {
   // Serial.println(inputString);
    if(inputString[0]=='d') /**< This is for debug and testing */
    {
      Serial.println(radio.readTemperature(1));
      Serial.println(msg_id);
    }

    msg_ret = sicl_process_internal_msg();

    if (msg_ret == 1)
    {
      ready2send=true;
    }
    else if (msg_ret == 5)
    {
      radio_picture_data_tx();
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }  

  if (radio.receiveDone())
  {
    radio_receive_message();
  }
  
  if( ready2send)
  {
    hk_get_housekeeping();
    radio_telemtery_tx();
    //send_endTransmission();
    ready2send=false;
  }
//  delay(500);
}

void serialEvent() 
{
  wdt_reset();
  if(isSerial_enabled)
  {
    while (Serial.available()) 
    {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') 
      {
        stringComplete = true;
      }
    }
  }
}


