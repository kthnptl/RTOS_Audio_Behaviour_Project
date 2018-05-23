/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen                                                       updated 5-23-18
// Course:          CMPE244
// Assignment:      Final Project - RGB LED Sound Behavior on a Skateboard
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////
// ledDriver.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ledDriver.hpp"

//Variables Used for Specific Modes
static uint8_t rainTemp, rgbTemp;

//LED - Array 0: Rainbow Spectrum - Array 1: Return Colors
uint8_t rByte[2] = {0xFF,0x00};
uint8_t gByte[2] = {0x00,0x00};
uint8_t bByte[2] = {0x00,0x00};

//data sent to SPI - LEDSTRIPS: specified LED array - LEDCOUNT: # of LED's per strip
uint8_t ledCnt, resetCnt;
uint32_t dataLed[LEDSTRIPS][LEDCOUNT]={0};

//default initialization for LED configuration
uint8_t speedTemp               = MEDIUMMODE;
uint8_t behaviorBtn             = 0;
uint8_t colorDiffBtn            = 30;
uint8_t colorBtn                = RAINBOW;
uint8_t modeBtn                 = RAINMODE;
uint8_t freqBand[FREQBANDS]     = {0}; 

void rainEffect(bool ledstrips)
{
    //////////////////////////////////////////////////////////////
    // temp variable used to assign to dataLED array
    //////////////////////////////////////////////////////////////
    static uint32_t data;

    //////////////////////////////////////////////////////////////
    // assign zero (off) to specified led's to create separation
    //////////////////////////////////////////////////////////////
    if(rainTemp >= behaviorBtn)
    {
        data = colorSpectrum(colorBtn);
        rainTemp = 0;
    }
    else
    {
        data = colorSpectrum(OFF);
        ++rainTemp;
    }

    //////////////////////////////////////////////////////////////
    // assign data color to led and create "rain effect"
    //////////////////////////////////////////////////////////////
    for(uint8_t loc = (LEDSPLIT-1); loc > 0; loc--)
    {
        dataLed[ledstrips][loc] = dataLed[ledstrips][loc-1];
    }
    dataLed[ledstrips][0]             = data;
     
    for(uint8_t loc = LEDSPLIT; loc < LEDCOUNT; loc++)
    {
        dataLed[ledstrips][loc-1] = dataLed[ledstrips][loc];
    }
    dataLed[ledstrips][LEDCOUNT-1]    = data;
}

void pulseEffect(bool ledstrips)
{
    if (colorBtn == 0 || colorBtn == 2)
    {
        ++colorBtn;
    }
    
    //////////////////////////////////////////////////////////////
    // updates all LED's to pulse
    //////////////////////////////////////////////////////////////
    for(uint8_t loc = 0; loc < LEDCOUNT; loc++)
    {
        dataLed[ledstrips][loc] = colorPulse(colorBtn);
    }
}

void soundEffect(bool ledstrips)
{
    static uint32_t data[2];
    
    data[0] = colorSpectrum(freqBand[1]+1);
    data[1] = colorSpectrum(freqBand[2]+1);
    
    for(uint8_t loc = (LEDSPLIT-1); loc > 0; loc--)
    {
        dataLed[ledstrips][loc] = dataLed[ledstrips][loc-1];
    }
    for(uint8_t loc = LEDSPLIT; loc < LEDCOUNT; loc++)
    {
        dataLed[ledstrips][loc-1] = dataLed[ledstrips][loc];
    }
    dataLed[ledstrips][0]             = data[0];
    dataLed[ledstrips][LEDCOUNT-1]    = data[1];
}

void equalizerEffect(bool ledstrips)
{
    uint8_t temp = LEDSPLIT/2;
    
    //////////////////////////////////////////////////////////////
    //Each "for" loop represents each Freq Band represented
    //////////////////////////////////////////////////////////////
    for(uint8_t loc = 0; loc < temp; loc++)
    {
        if(freqBand[0] > loc)
        {
            dataLed[ledstrips][loc] = colorSpectrum(colorBtn);
        }
        else
        {
            dataLed[ledstrips][loc] = colorSpectrum(OFF);
        }
    }

    for(uint8_t loc = temp; loc < LEDSPLIT; loc++)
    {
        if(freqBand[1] > (loc - temp))
        {
            dataLed[ledstrips][loc] = colorSpectrum(colorBtn);
        }
        else
        {
            dataLed[ledstrips][loc] = colorSpectrum(OFF);
        }
    }

    for(uint8_t loc = LEDSPLIT; loc < (LEDCOUNT-temp); loc++)
    {
        if((temp-freqBand[2]) < (loc - (LEDSPLIT)))
        {
            dataLed[ledstrips][loc] = colorSpectrum(colorBtn);
        }
        else
        {
            dataLed[ledstrips][loc] = colorSpectrum(OFF);
        }
    }

    for(uint8_t loc = LEDCOUNT-temp; loc < (LEDCOUNT); loc++)
    {
        if((temp-freqBand[3]) < (loc - (LEDCOUNT-temp)))
        {
            dataLed[ledstrips][loc] = colorSpectrum(colorBtn);
        }
        else
        {
            dataLed[ledstrips][loc] = colorSpectrum(OFF);
        }
    }
}

void colorRGB(void)
{
    uint8_t maxBound = 0xFF - (colorDiffBtn+1);
    uint8_t minBound = colorDiffBtn+1;
    
    switch(rgbTemp)
    {
        case 0: //red to yellow
            gByte[0]+=colorDiffBtn;
            if(gByte[0] >= maxBound)
            {
                ++rgbTemp;
                gByte[0] = 0xFF;
            }
            break;
        case 1: //yellow to green
            rByte[0]-=colorDiffBtn;
            if(rByte[0] <= minBound)
            {
                ++rgbTemp;
                rByte[0] = 0x00;
            }
            break;
        case 2: //green to teal
            bByte[0]+=colorDiffBtn;
            if(bByte[0] >= maxBound)
            {
                ++rgbTemp;
                bByte[0] = 0xFF;
            }
            break;
        case 3: //teal to blue
            gByte[0]-=colorDiffBtn;
            if(gByte[0] <= minBound)
            {
                ++rgbTemp;
                gByte[0] = 0x00;
            }
            break;
        case 4: //blue to purple
            rByte[0]+=colorDiffBtn;
            if(rByte[0] >= maxBound)
            {
                ++rgbTemp;
                rByte[0] = 0xFF;
            }
            break;
        case 5: //purple to red
            bByte[0]-=colorDiffBtn;
            if(bByte[0] <= minBound)
            {
                rgbTemp = 0;
                bByte[0] = 0x00;
            }
            break;
    }
}

uint32_t colorSpectrum(uint8_t color)
{
    switch(color)
    {
        case RAINBOW:
            colorRGB();
            rByte[1]= rByte[0];
            gByte[1]= gByte[0];
            bByte[1]= bByte[0];
            break;
        case RED:
            rByte[1]= 0xFF;
            gByte[1]= 0x00;
            bByte[1]= 0x00;
            break;
        case ORANGE:
            rByte[1]= 0xFF;
            gByte[1]= 0x50;
            bByte[1]= 0x00;
            break;
        case YELLOW:
            rByte[1]= 0xFF;
            gByte[1]= 0xFF;
            bByte[1]= 0x00;
            break;
        case GREEN:
            rByte[1]= 0x00;
            gByte[1]= 0xFF;
            bByte[1]= 0x00;
            break;
        case TEAL:
            rByte[1]= 0x00;
            gByte[1]= 0xFF;
            bByte[1]= 0xFF;
            break;
        case BLUE:
            rByte[1]= 0x00;
            gByte[1]= 0x00;
            bByte[1]= 0xFF;
            break;
        case PURPLE:
            rByte[1]= 0xFF;
            gByte[1]= 0x00;
            bByte[1]= 0xFF;
            break;
        case WHITE:
            rByte[1]= 0xFF;
            gByte[1]= 0xFF;
            bByte[1]= 0xFF;
            break;
        case OFF:
            rByte[1]= 0x00;
            gByte[1]= 0x00;
            bByte[1]= 0x00;
            break;
    }
    
    return (rByte[1]<<16) + (gByte[1]<<8) + bByte[1];
}

uint32_t colorPulse(uint8_t color)
{
    static bool flag = true;
    uint8_t maxBound = 0xFF - (colorDiffBtn+1);
    uint8_t minBound = colorDiffBtn+1;
    
    switch(color)
    {
        case RAINBOW:
            break;
        case RED:
            if(flag == true)
            {
                rByte[1]+=colorDiffBtn;
                if(rByte[1] >= maxBound){flag = false;}
            }
            else
            {
                rByte[1]-=colorDiffBtn;
                if(rByte[1] <= minBound){flag = true;}
            }
            gByte[1]= 0x00;
            bByte[1]= 0x00;
            break;
        case ORANGE:
            break;
        case YELLOW:
            if(flag == true)
            {
                rByte[1]+=colorDiffBtn;
                gByte[1]+=colorDiffBtn;
                if(rByte[1] >= maxBound){flag = false;}
            }
            else
            {
                rByte[1]-=colorDiffBtn;
                gByte[1]-=colorDiffBtn;
                if(rByte[1] <= minBound){flag = true;}
            }
            bByte[1]= 0x00;
            break;
        case GREEN:
            rByte[1]= 0x00;
            if(flag == true)
            {
                gByte[1]+=colorDiffBtn;
                if(gByte[1] >= maxBound){flag = false;}
            }
            else
            {
                gByte[1]-=colorDiffBtn;
                if(gByte[1] <= minBound){flag = true;}
            }
            bByte[1]= 0x00;
            break;
        case TEAL:
            rByte[1]= 0x00;
            if(flag == true)
            {
                gByte[1]+=colorDiffBtn;
                bByte[1]+=colorDiffBtn;
                if(gByte[1] >= maxBound){flag = false;}
            }
            else
            {
                gByte[1]-=colorDiffBtn;
                bByte[1]-=colorDiffBtn;
                if(gByte[1] <= minBound){flag = true;}
            }
            break;
        case BLUE:
            rByte[1]= 0x00;
            gByte[1]= 0x00;
            if(flag == true)
            {
                bByte[1]+=colorDiffBtn;
                if(bByte[1] >= maxBound){flag = false;}
            }
            else
            {
                bByte[1]-=colorDiffBtn;
                if(bByte[1] <= minBound){flag = true;}
            }
            break;
        case PURPLE:
            if(flag == true)
            {
                rByte[1]+=colorDiffBtn;
                bByte[1]+=colorDiffBtn;
                if(rByte[1] >= maxBound){flag = false;}
            }
            else
            {
                rByte[1]-=colorDiffBtn;
                bByte[1]-=colorDiffBtn;
                if(rByte[1] <= minBound){flag = true;}
            }
            gByte[1]= 0x00;
            break;
        case WHITE:
            if(flag == true)
            {
                rByte[1]+=colorDiffBtn;
                gByte[1]+=colorDiffBtn;
                bByte[1]+=colorDiffBtn;
                if(rByte[1] >= maxBound){flag = false;}
            }
            else
            {
                rByte[1]-=colorDiffBtn;
                gByte[1]-=colorDiffBtn;
                bByte[1]-=colorDiffBtn;
                if(rByte[1] <= minBound){flag = true;}
            }
            break;
        case OFF:
            rByte[1]= 0x00;
            gByte[1]= 0x00;
            bByte[1]= 0x00;
            break;
    }
    
    return (rByte[1]<<16) + (gByte[1]<<8) + bByte[1];
}

void flagCase(uint8_t speed)
{
    if(resetCnt == speed)
    {
        resetCnt = 0;
    }
    else if(resetCnt >= 1)
    {
        ++resetCnt;
    }
}

void dataInput(uint8_t x)
{
    //process data from WIFI - ADC Readings
    ADC_input temp;
    temp.byte = x;

    //store data into bands
    freqBand[temp.band] = temp.intensity;  //set intensity

    //adjust speed
    speedTemp = (!(temp.speed))*90 + 60;   //assign speed
}
