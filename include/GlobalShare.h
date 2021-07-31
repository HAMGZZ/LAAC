#pragma once

#define ROM_HOME_AZ 20
#define ROM_HOME_EL 30

#define UP_B 32
#define DOWN_B 30
#define RIGHT_B 31
#define LEFT_B 29
#define HOME_B 28

#define UP_Button       digitalRead(UP_B)
#define DOWN_Button     digitalRead(DOWN_B)
#define RIGHT_Button    digitalRead(RIGHT_B)
#define LEFT_Button     digitalRead(LEFT_B)
#define HOME_Button     digitalRead(HOME_B)

#define LED_OK_INTERVAL     2000
#define LED_WARN_INTERVAL   1000
#define LED_ERROR_INTERVAL  500
#define LED_HALT_INTERVAL   100

// var to keep track of LED times
int ledTimer;

// Type for tracking xy in 1 var
typedef struct t_xy
{
    double az;
    double el;
} t_xy;

// Once calibrated, this is true
bool calState;

bool travelOveride;

bool logSend;

bool EasyCommMode;

bool testFlag;

const int PROGMEM az_zero = 37;
const int PROGMEM az_oneeighty = 36; 
const int PROGMEM el_zero = 35;
const int PROGMEM el_ninety = 34;


t_xy rxAzEl;
volatile t_xy txAzEl;
t_xy home;
t_xy prevRx;