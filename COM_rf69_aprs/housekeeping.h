#ifndef HOUSEKEEPING_H
#define HOUSEKEEPING_H

#define AVRG                  100
#define INTERNALCALIB         ((double)(90.0 ))
#define INTERNALGAIN          ((double)(1.51))
#define CALIBRATION_OFFSET    ((double)(-20.18))
#define CALIBRATION_GAIN      ((double)(1.56))


volatile float temp = 0.0;
volatile float temp_avg = 0.0;

volatile double int_temp =0.0;
volatile double int_temp_avg =0.0;



#endif
