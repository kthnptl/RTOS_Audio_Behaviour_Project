/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen               SID: 007191860                       updated 5-20-18
// Course:          CMPE244
// Assignment:      GPIO/Task-Scheduler
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 1- GPIO Class Header File
//      - Class Constructor for GPIO configurations
//      - private contains GPIO pointer and port/pin #
//      - public contains function calls
//      - functions defined in LabGPIO_0.cpp
// 2- currently located in L5-App main.cpp folder
/////////////////////////////////////////////////////////////////////////////////////////////////////
 
#ifndef LABGPIO_0_HPP_
#define LABGPIO_0_HPP_

#include "LPC17xx.h"
#include <stdint.h>

class LabGPIO_0
{
    private:
        LabGPIO_0();                               // constructor (leave alone)
        const uint8_t port_Number;                 // port #
        volatile LPC_GPIO_TypeDef *GPIO_Ptr;       // GPIO pointer

    public:
        LabGPIO_0(uint8_t port);

        void setAsInput(uint8_t pin);

        void setAsOutput(uint8_t pin);

        void setDirection(bool output, uint8_t pin);

        void setHigh(uint8_t pin);

        void setLow(uint8_t pin);

        void set(bool high, uint8_t pin);

        bool getLevel(uint8_t pin);

        void setToggle(uint8_t pin);

        void pullUp(uint8_t pin);

        void pullDown(uint8_t pin);
    
        void ledOff(void);
        
        ~LabGPIO_0();
};

#endif /* LABGPIO_0_HPP_ */
