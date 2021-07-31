#pragma once
#include <Arduino.h>
#include "GlobalShare.h"
#include "Logger.h"
#include "AccelStepper.h"
#include "math.h"

#include <TeensyThreads.h>

#define PLUSMINUS_RANGE 5
#define MAX_SPEED 1000
#define MAX_ACCELERATION 50
#define AZ_START_SPD 15
#define EL_START_SPD 15


AccelStepper AZ_Stepper(1, 16, 17);
AccelStepper EL_Stepper(1, 18, 19);

Logger rotorLog("Rotor", INFO);


int azStepsPerDegree;
int elStepsPerDegree;
void moveThread();

class Rotor
{
private:

    bool checkAz(float az);
    bool checkEl(float el);

public:
    Rotor();
    void UpdatePosition();
    void Callibrate();
    void Home();
    
    ~Rotor();
};

Rotor::Rotor()
{
    calState = false;
    rotorLog.Send(INFO, "Initialsing...");
    AZ_Stepper.setMaxSpeed(MAX_SPEED);
    rotorLog.Send(DEBUG, "AZ_Stepper Max speed set");
    AZ_Stepper.setAcceleration(MAX_ACCELERATION);
    rotorLog.Send(DEBUG, "AZ_Stepper Max acceleration set");
    AZ_Stepper.setMinPulseWidth(20);
    rotorLog.Send(DEBUG, "AZ_Stepper min pulse width set");
    rotorLog.Send(INFO, "AZ_Stepper initialised");
    EL_Stepper.setMaxSpeed(MAX_SPEED);
    rotorLog.Send(DEBUG, "EL_Stepper Max speed set");
    EL_Stepper.setAcceleration(MAX_ACCELERATION);
    rotorLog.Send(DEBUG, "EL_Stepper Max acceleration set");
    EL_Stepper.setMinPulseWidth(20);
    rotorLog.Send(DEBUG, "EL_Stepper min pulse width set");
    rotorLog.Send(INFO, "ELs_Stepper initialised");
    azStepsPerDegree = AZ_START_SPD;
    elStepsPerDegree = EL_START_SPD;
    rotorLog.Send(INFO, "Initialised");
    rotorLog.Send(INFO, "Starting Thread");
    threads.addThread(moveThread, 1);
}

void Rotor::Callibrate()
{
    AZ_Stepper.setMaxSpeed(10);
    EL_Stepper.setMaxSpeed(10);
    // AZ & EL calibration loop
    bool azZero = false;
    bool elZero = false;
    bool azTop = false;
    bool elTop = false;
    while(!calState)
    {
        if(!digitalRead(az_zero) && !azZero)
        {
            AZ_Stepper.move(-1);
        }
        else if(!elZero)
        {
            AZ_Stepper.stop();
            AZ_Stepper.setCurrentPosition(0);
            azZero = true;
        }

        if(!digitalRead(el_zero) && !elZero)
        {
            EL_Stepper.move(-1);
        }
        else if(!elZero)
        {
            EL_Stepper.stop();
            EL_Stepper.setCurrentPosition(0);
            elZero = true;
        }

        if(!digitalRead(az_oneeighty) && azZero)
        {
            AZ_Stepper.move(1);
        }
        else if(!azTop && azZero)
        {
            AZ_Stepper.stop();
            azStepsPerDegree = AZ_Stepper.currentPosition() / 180;
        }

        if(!digitalRead(el_ninety) && elZero)
        {
            EL_Stepper.move(1);
        }
        else if(!elTop && elZero)
        {
            EL_Stepper.stop();
            elStepsPerDegree = EL_Stepper.currentPosition() / 90;
        }

        if(azZero && azTop && elZero && elTop)
        {
            calState = true;
            rxAzEl = home;
            AZ_Stepper.setMaxSpeed(MAX_SPEED);
            EL_Stepper.setMaxSpeed(MAX_SPEED);
        }
    }

}

void moveThread()
{
    while(1)
    {
        if(calState)
        {
            if(rxAzEl.az != prevRx.az)
            {

                long stepsRequired = rxAzEl.az * azStepsPerDegree;

                rotorLog.Send(DEBUG, "AZ Steps required = ", stepsRequired);
                AZ_Stepper.moveTo(stepsRequired);
                prevRx.az = rxAzEl.az;
            }
            if(rxAzEl.el != prevRx.el)
            {
                long stepsRequired2 = rxAzEl.el*elStepsPerDegree;
                rotorLog.Send(DEBUG, "EL Steps required = ", stepsRequired2);
                EL_Stepper.moveTo(stepsRequired2);
                prevRx.el = rxAzEl.el;
            }
        }
        EL_Stepper.run();
        txAzEl.el = EL_Stepper.currentPosition() / elStepsPerDegree;
        AZ_Stepper.run();
        txAzEl.az = AZ_Stepper.currentPosition() / azStepsPerDegree;
        delay(1);
    }
    
}

/*
void Rotor::UpdatePosition()
{
    if(rxAzEl.az != prevRx.az)
        {

            long stepsRequired = rxAzEl.az * azStepsPerDegree;

            rotorLog.Send(DEBUG, "AZ Steps required = ", stepsRequired);
            AZ_Stepper.moveTo(stepsRequired);
            prevRx.az = rxAzEl.az;
        }
        if(rxAzEl.el != prevRx.el)
        {
            long stepsRequired2 = rxAzEl.el*elStepsPerDegree;
            rotorLog.Send(DEBUG, "EL Steps required = ", stepsRequired2);
            EL_Stepper.moveTo(stepsRequired2);
            prevRx.el = rxAzEl.el;
        }
        EL_Stepper.run();
        txAzEl.el = EL_Stepper.currentPosition() / elStepsPerDegree;
        AZ_Stepper.run();
        txAzEl.az = AZ_Stepper.currentPosition() / azStepsPerDegree;
}*/


Rotor::~Rotor()
{
    rotorLog.Send(INFO, "Class deleted");
}
