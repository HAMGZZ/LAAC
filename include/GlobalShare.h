#pragma once

#define ROM_HOME_AZ 20
#define ROM_HOME_EL 30

#define UP_B 32
#define DOWN_B 30
#define RIGHT_B 31
#define LEFT_B 29
#define HOME_B 28

#define UP_Button       digitalRead(32)
#define DOWN_Button     digitalRead(30)
#define RIGHT_Button    digitalRead(31)
#define LEFT_Button     digitalRead(29)
#define HOME_Button     digitalRead(28)

typedef struct t_xy
{
    double az;
    double el;
} t_xy;

bool travelOveride;

bool logSend;

bool EasyCommMode;

bool testFlag;

const int PROGMEM az_zero = 37;
const int PROGMEM az_oneeighty = 36; 
const int PROGMEM el_zero = 35;
const int PROGMEM el_ninety = 34;


t_xy rxAzEl;
t_xy txAzEl;
t_xy home;
t_xy prevRx;