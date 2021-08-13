#include <Arduino.h>
#include "GlobalShare.h"
#include "Logger.h"
#include <SPI.h>

#define MAX7219_CS 10
#define MAX7219_DIN 11
#define MAX7219_CLK 13

Logger displayLog("Screen", DEBUG);

class Screen
{
private:
    void set_register(byte reg, byte value);
    void resetDisplay();
    const byte DP = 0b10000000;  
    const byte C  = 0b01001110;  
    const byte F  = 0b01000111;
    enum {  MAX7219_REG_DECODE    = 0x09,  
        MAX7219_REG_INTENSITY = 0x0A,
        MAX7219_REG_SCANLIMIT = 0x0B,
        MAX7219_REG_SHUTDOWN  = 0x0C,
        MAX7219_REG_DISPTEST  = 0x0F };
    char prevBuf1[4] = {0};
    char prevBuf2[4] = {0};
public:
    Screen();
    void write(int number1[4], int number2[4], bool dotpoint = true);
    void stringTo4Digit(char s[], int *digits[]);
    void update();
    ~Screen();
};

Screen::Screen()
{
    pinMode(MAX7219_DIN, OUTPUT);   // serial data-in
    pinMode(MAX7219_CS, OUTPUT);    // chip-select, active low    
    pinMode(MAX7219_CLK, OUTPUT);   // serial clock
    digitalWrite(MAX7219_CLK, HIGH);
    resetDisplay(); 
}

void Screen::string  To4Digit(char s[], int *digits[])
{
    
}

void Screen::update()
{
    char buffer1[4] = {0};
    char dem1[1] = {0};
    char buffer2[4] = {0};
    char dem2[1] = {0};
    int number1[4] = {0};
    int number2[4] = {0};
    sprintf(buffer1, "%03.0f", txAzEl.az);
    sprintf(dem1, "%1.0f", txAzEl.az-floor(txAzEl.az) * 10);
    sscanf(buffer1, "%d", &number1[0]);
    sscanf(dem1, "%d", &number1[3]);

    strcat(buffer1, dem1);
    sprintf(buffer2, "%03.0f", txAzEl.el);
    sprintf(dem2, "%1.0f", txAzEl.el-floor(txAzEl.el) * 10);
    strcat(buffer2, dem2);

    if(buffer1 != prevBuf1 || buffer2 != prevBuf2)
    {
        int testDisplay[2][4] = {{8,8,8,8}, {8,8,8,8}};
        write(testDisplay[0], testDisplay[1]);
        strcpy(prevBuf1, buffer1);
        strcpy(prevBuf2, buffer2);
    }
}

void Screen::set_register(byte reg, byte value)  
{
    digitalWrite(MAX7219_CS, LOW);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, reg);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, value);
    digitalWrite(MAX7219_CS, HIGH);
}


void Screen::write(int number1[4], int number2[4], bool dotpoint = true)
{
    set_register(MAX7219_REG_SHUTDOWN, 0);      // turn off display
    set_register(MAX7219_REG_SCANLIMIT, 7);       // limit to 8 digits
    set_register(MAX7219_REG_DECODE, 0xff); // decode all digits

    set_register(1, number1[3]);
    set_register(2, number1[2]);
    set_register(3, number1[1] | (dotpoint ? DP : 0b00000000));
    set_register(4, number1[0]);
    set_register(5, number2[3]);
    set_register(6, number2[2]);
    set_register(7, number2[1] | (dotpoint ? DP : 0b00000000));
    set_register(8, number2[0]);

    set_register(MAX7219_REG_SHUTDOWN, 1);       // Turn on display
}

void Screen::resetDisplay()  
{
    set_register(MAX7219_REG_DISPTEST, 0);   // turn off test mode
    set_register(MAX7219_REG_SHUTDOWN, 1);   // turn off display
    set_register(MAX7219_REG_SCANLIMIT, 7);
    set_register(MAX7219_REG_DECODE, 0xFF);
    set_register(MAX7219_REG_INTENSITY, 0x0F); // display intensity
}

Screen::~Screen()
{
}
