#ifndef BUS_H
#define BUS_H

//#define debug1
//#define debug2

#define SICL_BAUD     9600    /**< SICL Bus baudrade */
#define SICLRX        0       /**< RX pin */
#define SICLTX        1       /**< TX pin */
#define BUSBUSY       A1      /**< Bus Busy pin */


/*
 * Default telemetry data initialization
 */
char GPS_time[10] = {'3', '3', '6', '6', '7', '7', '\0'};
char GPS_lati[12] = {'+', '0', '0', '0', '0', '.', '0', '0', '0', '\0'};
char GPS_long[13] = {'+', '0', '0', '0', '0', '0', '.', '0', '0', '0', '\0'};
char GPS_alti[8] = { '9','9','9','9','9', '\0' };
char EXT_temp[6] = { '9','9','9','9', '\0' };
char OBC_temp[6] = { '9','9','9','9', '\0' };
char COM_temp[6] = { '9','9','9','9', '\0' };


char input=0;
char incoming=0;
char msg[66];
char msg2[66];
bool isSerial_enabled;
int msg_ret;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int msg_id=0;
boolean ready2send = false;



#endif
