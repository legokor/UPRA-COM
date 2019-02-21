#ifndef RADIO_H
#define RADIO_H

#define NETWORKID     100  //the same on all nodes that talk to each other
#define NODEID        1  
#define RECEIVER      2    // The recipient of packets - nodeid on the other hw

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY     "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

/*
 * Select proper TCVR module
 */
//REG_FRFMSB    COM.RF69HCW-1: 6C | COM.RF69HCW-2: 6C | COM.RF69CW-1: 6C | COM.RF69CW-2: 6C
//REG_FRFMID    COM.RF69HCW-1: 40 | COM.RF69HCW-2: 40 | COM.RF69CW-1: 42 | COM.RF69CW-2: 6C
//REG_FRFLSB    COM.RF69HCW-1: 00 | COM.RF69HCW-2: 00 | COM.RF69CW-1: C0 | COM.RF69CW-2: 6C
//#define COM_RF69HCW1
#define COM_RF69HCW2
//#define COM_RF69CW1
//#define COM_RF69CW2

/*
 * TCVR Module Pins
 */
#define RFM69_CS      10
#define RFM69_IRQ     2
#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     9


int transmit_error = 0;
char callsign[7];
char new_callsign[7];
int last_msg_RSSI=0;

unsigned  char     telemetry[66]= {
  '$', '$', 'H', 'A', 'x', 'U', 'P', 'R', 'A', ',',                        //MSG HEADER      10
  '0', '0', '0', ',',                                                      //11-MSG ID          4 
  '3', '3', '6', '6', '7', '7', ',',                                       //15-GPS TIME        7
  '+', '0', '0', '0', '0', '.', '0', '0', '0', ',',                        //22-GPS LATITUDE    10
  '+', '0', '0', '0', '0', '0', '.', '0', '0', '0', ',',                   //32-GPS LONGITUDE   11
  '9','9','9','9','9', ',',                                                //43-GPS ALTITUDE    6
  '9','9','9','9', ',',                                                    //48-EXTERNAL TEMP   5
  '9','9','9', ',',                                                        //53-OBC TEMP        4
  '9','9','9', ',',                                                        //57-COM TEMP        4
};


RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);


#endif
