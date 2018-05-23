/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen                                                       updated 5-5-18
// Course:          CMPE244
// Assignment:      Final Project
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Description:
//  - TC Driver Header File
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TCDRIVER_HPP_
#define TCDRIVER_HPP_

#include "LPC17xx.h"

//print header
#include "printf_lib.h"

#define TIMERZERO   0

class tcDriver
{
    private:
        uint32_t pClkFreq;
        uint32_t mrNum;
        const uint8_t  tNum;
        volatile LPC_TIM_TypeDef *LPC_TIMER_PTR;
    
    public:
        enum lpcTimerNum
        {
            timerZero,
            timerOne,
            timerTwo,
            timerThree,
        };
    
        tcDriver(uint8_t timerNum);
        
        //enable TC interrupt
        void tcEnableIRQ(void);
        
        //clear interrupt flag for TC#
        bool tcClrIntFlag(void);
        
        //init TC
        void tcInit(uint32_t freq);
        
        void setMR(uint32_t freq); 
        
        ~tcDriver(void);
};

#endif /* TCDRIVER_HPP_ */
