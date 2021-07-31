#pragma once

#include <Arduino.h>
#include "TimeLib.h"
#include "GlobalShare.h"


typedef enum
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
} Level;

class Logger
{
private:
    char *name;
    Level minLogLevel;
    char const *enum_to_string(Level type);
    time_t getTeensyTime();
public:
    Logger(char const *SystemName, Level minLogLevelSet);
    void Send(Level level, char const *message, float var1 = (0.0F), float var2 = (0.0F));
    ~Logger();
};

Logger::Logger(char const *systemName, Level minLogLevelSet)
{
    name = new (char[30]);
    strcpy(name, systemName);
    minLogLevel = minLogLevelSet;
}


char const *Logger::enum_to_string(Level type) {
   switch(type) {
      case DEBUG:
         return "DEBUG";
      case INFO:
         return "INFO";
      case WARNING:
         return "WARNING";
      case ERROR:
         return "ERROR";
      case FATAL:
         return "FATAL";
   }
   return "";
}

void Logger::Send(Level level, const char *message, float var1 = (0.0F), float var2 = (0.0F))
{
    if(logSend)
    {
        if(level < minLogLevel)
            return;
    
        
        if(var1 == 0 && var2 == 0)
        {
            Serial.printf("LOG [%02d/%02d/%d %02d:%02d:%02d][%s][%s]::  %s\n\r", day(), month(), year(), hour(), minute(), second(), name, enum_to_string(level), message);
        }
    
        else if(var2 == 0)
        {
            Serial.printf("LOG [%02d/%02d/%d %02d:%02d:%02d][%s][%s]::  %s%lf\n\r", day(), month(), year(), hour(), minute(), second(), name, enum_to_string(level), message, var1);
        }
        else
        {
            Serial.printf("LOG [%02d/%02d/%d %02d:%02d:%02d][%s][%s]::  %s%lf %lf\n\r", day(), month(), year(), hour(), minute(), second(), name, enum_to_string(level), message, var1, var2);
        }
        
        
        if(level == FATAL)
        {
            Serial.printf("\n\n\rHALTING...\n\n\r");
            delay(100);
            while(1)
            {
                digitalWrite(13, 1);
                delay(50);
                digitalWrite(13, 0);
                delay(100);
                digitalWrite(13, 1);
                delay(50);
                digitalWrite(13, 0);
                delay(1000);
            }
        }
    }

}


Logger::~Logger()
{
    delete[] name;
}


