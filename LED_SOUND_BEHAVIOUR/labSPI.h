/////////////////////////////////////////////////////////////////////////////////////////////////////
// Author:          Alan Chen                                                        updated 5-23-18
// Course:          CMPE244
// Assignment:      SPI
// Instructor:      Preetpal Kang
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LABSPI_H_
#define LABSPI_H_

#include "LPC17xx.h"
#include "printf_lib.h"
#include <stdio.h>
#include "utilities.h"
#include "io.hpp"
#include "FreeRTOS.h"
#include "semphr.h"

#define SPI_BIT_SIZE    8
#define SPI_CLK_DIVIDE  1

enum FrameModes
{
    fSPI         = 0b00,
    fTI          = 0b01,
    fMicrowire   = 0b10
};

enum Peripheral
{
    SSP0 = 0,
    SSP1 = 1
};

class labSPI
{
    private:
        labSPI();
        volatile LPC_SSP_TypeDef * SSPtr;
        const uint8_t sspNum;

    public:
        labSPI(Peripheral spiNum);

        /**
         * 1) Powers on SPPn peripheral
         * 2) Set peripheral clock
         * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
         *
         * @param peripheral which peripheral SSP0 or SSP1 you want to select.
         * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the data sheet
         * @param format is the code format for which synchronous serial protocol you want to use.
         * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
         *
         * @return true if initialization was successful
         */
        bool init(uint8_t data_size_select, FrameModes format, uint8_t divide);

        /**
         * Transfers a byte via SSP to an external device using the SSP data register.
         * This region must be protected by a mutex static to this class.
         *
         * @return received byte from external device via SSP data register.
         */
        uint8_t transfer(uint8_t send);
        
        ~labSPI(void);
    };

#endif /* LABSPI_H_ */
