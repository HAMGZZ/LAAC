#include <Arduino.h>
#include "logger.h"
#include "TimeLib.h"
#include "GlobalShare.h"
#include "Rotor.h"
#include "Screen.h"
#include "Communication.h"
#include <avr/io.h>
#include <avr/interrupt.h>


time_t getTeensyTime();

uint32_t FreeMem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

void healthChecks()
{
    Logger rootLog("ROOT", INFO);
    uint32_t free = FreeMem();
    if(free < 2000)
    {
        rootLog.Send(FATAL, "OUT OF MEMORY! FREE: ", free);
    }
    rootLog.Send(DEBUG, "Current free mem. :", free);
    uint32_t temp = tempmonGetTemp();
    if(temp > 80)
    {
        rootLog.Send(FATAL, "CPU OVERHEAT! TEMP: ", temp);
    }
    rootLog.Send(DEBUG, "Current temp. :", temp);
    if(second()-ledTimer > LED_OK_INTERVAL)
    {
        ledTimer = second(); 
        digitalWrite(13, !digitalRead(13));
    }

}

void buttonCheck()
{
    if(UP_Button)
    {
        rxAzEl.el += 5;
        delay(200);
    }
    if(DOWN_Button)
    {
        rxAzEl.el -= 5;
        delay(200);
    }
    if(LEFT_Button)
    {
        if(rxAzEl.az < 5)
            rxAzEl.az = (360 + rxAzEl.az) - 5;
        rxAzEl.az -= 5;
        delay(200);
    }
    if(RIGHT_Button)
    {
        if(rxAzEl.az >= 355)
            rxAzEl.az = (rxAzEl.az - 360) + 5;
        rxAzEl.az += 5;
        delay(200);
    }
    if(HOME_Button)
    {
        rxAzEl = home;
        delay(200);
    }
}


void setup()
{
    logSend = true;
    Serial.begin(115200);
    Serial.printf("Booting...\n\r");
    pinMode(13, OUTPUT);
    pinMode(33, INPUT);
    pinMode(UP_B, INPUT);
    pinMode(DOWN_B, INPUT);
    pinMode(LEFT_B, INPUT);
    pinMode(RIGHT_B, INPUT);
    pinMode(HOME_B, INPUT);
    
}

void loop()
{
    rxAzEl.az = 0;
    rxAzEl.el = 0;
    txAzEl.az = 0;
    txAzEl.el = 0;

    Communication com1(0);
    Logger mainLoopLog("Main", INFO);    
    mainLoopLog.Send(INFO, "Starting LAAC - Large Antenna Array Controller - DEATHBOX");
    setSyncProvider(getTeensyTime);
    if (timeStatus()!= timeSet) 
    {
      mainLoopLog.Send(ERROR, "Unable to sync with the RTC");
    } 
    else
    {
      mainLoopLog.Send(INFO, "RTC has set the system time");
    }

    Rotor rotor;
    Screen screen;
    screen.write("8888", "8888");

    mainLoopLog.Send(INFO, "System Started");
    mainLoopLog.Send(INFO, "Calibrating rotor");
    //rotor.Callibrate();
    mainLoopLog.Send(INFO, "Calibrated rotor");
    com1.send(  "Rotor controller prompt  Copyright (C) 2021  Lewis Hamilton\n\r"
                "This program comes with ABSOLUTELY NO WARRANTY; for details type `warranty'.\n\r"
                "Enter E to turn on console echo and console logging.\n\r");
    logSend = false;
    delay(100);
    while(1)
    {
        com1.console();
        screen.update();
        healthChecks();
        buttonCheck();
    }
}



time_t getTeensyTime()
{
    return Teensy3Clock.get();
}


