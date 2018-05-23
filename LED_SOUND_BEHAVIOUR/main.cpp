/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen                                                       updated 5-23-18
// Course:          CMPE244
// Assignment:      Final Project - RGB LED Sound Behavior on a Skateboard
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
//      1- 5 Tasks
//          - sound_input 
//              i - changes colors based on GPIO Input or Nordic Receive
//              ii- sends queue to transmit_LED (sends SPI data)
//          - transmit_LED (SPI data sent)
//              i - transmit to SPI0/1 (mosi/clk)
//              ii- sends different data if in equalizer mode
//          - button_mode
//              i - check SW0-4 input for LED changes
//              ii- de-bouncing is accounted for
//          - Wireless
//              i - wireless setup
//          - Nordic Receive
//              i - receive function used for equalizer and sound mode
//              ii- received data from SJone board (expecting 4 bytes per packet)
//      2- One TC interrupt used to trigger Latching (~666*N (us))
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Headers, Functions, and Global Initializations
/////////////////////////////////////////////////////////////////////////////////////////////////////
//C/C++ headers and Temp. headers
#include <stdio.h>
#include <stdlib.h>
#include "io.hpp"
#include "tasks.hpp"

//print header
#include "printf_lib.h"

//freeRTOS assignment headers
#include "FreeRTOS.h"

//Required LED Library Driver(s)
#include "labSPI.h"
#include "tcDriver.hpp"
#include "LabGPIO_0.hpp"
#include "ledDriver.hpp"

//Communication Libraries for Nordic
#include "wireless.h"

//RTOS tasks
void sound_input(void *p);
void transmit_LED(void *p);
void button_mode(void *p);
void nordic_rx(void *p);

//Driver Initializations
labSPI spiLED1(SSP1);
QueueHandle_t queueS;
tcDriver rgbPeriod(TIMERZERO);

//Interrupt period (~666us = 1/1500 Hz)
void timerZeroHandlerPer(void)
{
    flagCase(speedTemp);        //flag set or reset
    rgbPeriod.tcClrIntFlag();   //clear interrupt flag
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    //Stack Memory Size
    const uint32_t STACK_SIZE   = 1024;
    
    //create queue
    queueS = xQueueCreate(LEDCOUNT,sizeof(uint32_t));

    //simply to turn off SJone LED off quickly/default (only works if "1")
    LabGPIO_0 led(1);
    led.ledOff();

    //SPI initialization
    spiLED1.init(SPI_BIT_SIZE, fSPI, SPI_CLK_DIVIDE);
    
    //ISR function initialization
    isr_register(TIMER0_IRQn, timerZeroHandlerPer);

    //Initialize TC interrupt
    rgbPeriod.tcInit(4000000);  //4 MHz
    rgbPeriod.setMR(1500);      //1500 Hz
    rgbPeriod.tcEnableIRQ();

    //Tasks and Priorities assigned to Scheduler
    xTaskCreate(sound_input, (const char*)"input_sound", STACK_SIZE*2, 0, 2, 0);    //medium priority
    xTaskCreate(transmit_LED, (const char*)"output_pwm", STACK_SIZE, 0, 2, 0);      //medium priority
    xTaskCreate(button_mode, (const char*)"button_input", STACK_SIZE, 0, 3, 0);     //high priority
    xTaskCreate(nordic_rx, (const char*)"wireless send", STACK_SIZE*2, 0, 3, 0);    //high priority
    scheduler_add_task(new wirelessTask(PRIORITY_MEDIUM));                          //medium priority
    
    //Start Schedulers
    scheduler_start();
    
    return 0;   
}

void nordic_rx(void * par )
{
    //initialize packet for nordic
    mesh_packet_t packet;

    //static uint8_t count=0;
    
    if(!wireless_init())
    {
        printf("Wireless Init Unsuccessfull...\n");
    }

    while(1)
    {
        if((modeBtn == EQUALIZERMODE) || (modeBtn == SOUNDMODE))
        {
            if(wireless_get_rx_pkt(&packet,portMAX_DELAY))
            {
                //store data and process information
                dataInput(packet.data[0]);     // Hz
                dataInput(packet.data[1]);     // Hz
                dataInput(packet.data[2]);     // Hz
                dataInput(packet.data[3]);     // Hz
                //++count;
            }
        }
        vTaskDelay(10);
    }
}

void sound_input(void *p)
{
    //////////////////////////////////////////////////////////////
    // Variable Initialization for Queue
    //////////////////////////////////////////////////////////////
    static bool dataUpdate  = true;     //data is needs updating
    static bool stripNum    = 0;        //specified LED strip
    
    while(1)
    {
        //////////////////////////////////////////////////////////////
        // Queue Data upon Latch
        //////////////////////////////////////////////////////////////
        if(resetCnt == 0)
        {
            if(dataUpdate)
            {
                switch(modeBtn)
                {
                    case RAINMODE:
                        rainEffect(stripNum);
                        break;
                    case PULSEMODE:
                        colorDiffBtn = 1;
                        pulseEffect(stripNum);
                        break;
                    case SOUNDMODE:
                        
                        soundEffect(stripNum);
                        break;
                    case EQUALIZERMODE:
                        speedTemp = MEDIUMMODE;
                        equalizerEffect(stripNum);
                        break;
                }
                dataUpdate = false;
            }
            else
            {
                if(xQueueSend(queueS, &(dataLed[stripNum][ledCnt]), 0))
                {
                    ++ledCnt;
                    if(ledCnt > (LEDCOUNT-1))
                    {
                        resetCnt    = 1;
                        ledCnt      = 0;
                        dataUpdate  = true;
                    }
                }
            }
        }
        vTaskDelay(0);
    }
}

void transmit_LED(void *p)
{
    static uint32_t data;
    static uint8_t rBits, gBits, bBits;
    
    while(1)
    {
        //////////////////////////////////////////////////////////////
        //Transmit RGB Data from Queue
        //////////////////////////////////////////////////////////////
        if(!xQueueReceive(queueS, &data, portMAX_DELAY))
        {
            u0_dbg_printf("\nDid not receive");
        }
        else
        {
            rBits = (data >> 16)    & 0xFF;
            gBits = (data >> 8)     & 0xFF;
            bBits = data            & 0xFF;
            spiLED1.transfer(bBits);
            spiLED1.transfer(gBits);
            spiLED1.transfer(rBits);
        }
        
        vTaskDelay(0);
    }
}

void button_mode(void *)
{
    //////////////////////////////////////////////////////////////
    //Variable Initilizations to check for De-bouncing
    //////////////////////////////////////////////////////////////
    static uint8_t state = 0, pos = 0, x = 0;
    static uint8_t button[4]= {9,10,14,15};
    static bool flag        = false;

    //////////////////////////////////////////////////////////////
    //initialize input registers
    //////////////////////////////////////////////////////////////
    LabGPIO_0 buttons(1);
    for(x=0; x<4; x++)
    {
        buttons.setAsInput(button[x]);
        buttons.setLow(button[x]);
    }

    while(1)
    {
        //////////////////////////////////////////////////////////////
        // Button Check for States w/ De-bouncing
        //////////////////////////////////////////////////////////////
        if(buttons.getLevel(button[x]))
        {
            pos |= (1<<x);
        }
        else if(!(buttons.getLevel(button[x])) && (pos & (1<<x)))
        {
            pos &= ~(1<<x);
            state = x;
            flag = true;
        }
        
        if(x == 0)
        {
            x = 4;
        }
        --x;
        
        //////////////////////////////////////////////////////////////
        // Button Response
        //////////////////////////////////////////////////////////////
        if (flag == true)
        {
            switch(state)
            {
                case 0:         //mode (SW0)
                    ++modeBtn;
                    //reset parameters upon past equalizer mode
                    if(modeBtn == 4)
                    {
                        colorDiffBtn    = 30;
                        modeBtn         = 0;
                        behaviorBtn     = 0;
                        speedTemp       = SLOWMODE;
                    }
                    break;
                case 1:         //color (SW1)
                    ++colorBtn;
                    rByte[1] = 0;
                    gByte[1] = 0;
                    bByte[1] = 0;
                    if(colorBtn == 10)
                    {
                        colorBtn = RAINBOW;
                    }
                    break;

                case 2:         //led separation (SW2)
                    ++behaviorBtn;
                    if(behaviorBtn >= (LEDSPLIT-1))
                    {
                        behaviorBtn = 0;
                    }
                    break;
                case 3:         //speed and color separation (SW3)
                    if(colorBtn == RAINBOW)
                    {
                        colorDiffBtn += 20;
                        if(colorDiffBtn >= 0xCA)
                        {
                            colorDiffBtn = 2;
                        }
                    }
                    speedTemp -= 20;
                    if(speedTemp <= 30)
                    {
                        speedTemp = SLOWESTMODE;
                    }
                    break;
            }
            flag = false;
        }
        vTaskDelay(20);
    }
}
