/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen                                                       updated 5-23-18
// Course:          CMPE244
// Assignment:      Final Project - RGB LED Sound Behavior on a Skateboard
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////
// ledDriver.hpp
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LEDDRIVER_HPP_
#define LEDDRIVER_HPP_

//libraries
#include <stdint.h>
#include "printf_lib.h"

//Definitions for # of LEDs and Sound Inputs
#define LEDCOUNT    32
#define LEDSPLIT    16      //half of LEDCOUNT
#define LEDSTRIPS   2
#define FREQBANDS   4

enum behaviorModes
{
    RAINMODE,
    PULSEMODE,
    SOUNDMODE,
    EQUALIZERMODE
};

enum speedModes
{
    FASTMODE    = 20,
    MEDIUMMODE  = 90,
    SLOWMODE    = 150,
    SLOWESTMODE = 200
};

enum colorModes
{
    RAINBOW,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    TEAL,
    BLUE,
    PURPLE,
    WHITE,
    OFF
};

//bit field for Data Received from Nordic
typedef union
{
    struct
    {
        uint8_t band        :2;
                                    //00: for strip #1 -> either 63Hz  or  160Hz,
                                    //01: for strip #2 -> either 400Hz or  1kHz,
                                    //10: for strip #3 -> either 2.5kHz, 6.25kHz
                                    //11: for strip #4 -> for    16kHz
        uint8_t resvd0      :1;     //reserved
        uint8_t speed       :1;
                                    //0: slow
                                    //1: fast
        uint8_t intensity   :3;     //shows the % in the range from 0-7
        uint8_t resvd1      :1;     //reserved
    }__attribute__((packed));
    uint8_t byte;
}ADC_input;

//data referenced from ledDriver.cpp
extern uint8_t speedTemp, ledCnt, resetCnt;
extern uint32_t dataLed[LEDSTRIPS][LEDCOUNT];
extern uint8_t colorDiffBtn, behaviorBtn, colorBtn, modeBtn;
extern uint8_t freqBand[FREQBANDS];
extern uint8_t rByte[2], gByte[2], bByte[2];

///////////////////////////////////////////////////////////////////////////////////////////////
//color functions
///////////////////////////////////////////////////////////////////////////////////////////////
void colorRGB(void);                    //computes rainbow spectrum
uint32_t colorPulse(uint8_t color);     //computes pulsing effect
uint32_t colorSpectrum(uint8_t color);  //assign color spectrum

///////////////////////////////////////////////////////////////////////////////////////////////
//LED behaviour modes
///////////////////////////////////////////////////////////////////////////////////////////////
void rainEffect(bool ledstrips);        //separation effect
void pulseEffect(bool ledstrips);       //pulsing LED effect
void soundEffect(bool ledstrips);       //reacts to two bands with rain effect
void equalizerEffect(bool ledstrips);   //equalizer of four bands

///////////////////////////////////////////////////////////////////////////////////////////////
//interrupt flag checks
///////////////////////////////////////////////////////////////////////////////////////////////
void flagCase(uint8_t speed);

///////////////////////////////////////////////////////////////////////////////////////////////
//un-pack data received
///////////////////////////////////////////////////////////////////////////////////////////////
void dataInput(uint8_t x);

#endif /* LEDDRIVER_HPP_ */
