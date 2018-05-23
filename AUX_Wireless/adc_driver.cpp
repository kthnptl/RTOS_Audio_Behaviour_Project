#include "adc_driver.h"
#include "utilities.h"

ADCDriver::ADCDriver(){
}

void ADCDriver::adcInitBurstMode()
{
    LPC_SC->PCONP |= (1<<12);  //setting bit no.12 for Powering up ADC
    LPC_SC->PCLKSEL0 &= ~(3<<24); //clear bit 25 and 24 for clock selection
    LPC_SC->PCLKSEL0 |= (2<<24); //clk = cpu_clk/2; 48/2 = 24 MHz
    LPC_ADC->ADCR |= (1<<9); // set bit no.9 for further clk div to 2; 24/2 = 12 MHz
    LPC_ADC->ADCR |= (1<<16); //ENABLE BURST MODE
    LPC_ADC->ADCR |= (1<<21);  //setting PDN bit in AD control Register
    //CLEAR ALL OTHER CHANNELS
    LPC_ADC->ADCR &= ~(0xFF); //clr all channels
}

void ADCDriver::adcSelectPin(ADC_PIN adc_pin_arg)
{
    switch(adc_pin_arg)
    {
        case ADC_PIN_0_25:  LPC_PINCON->PINSEL1 |= (1<<18); // set p0.25 as adc2
                            LPC_PINCON->PINMODE1 |= (2<<18); // set NO PULL UP OR PULL DOWN REST
                            LPC_ADC->ADCR |= (1<<2); // set bit no. 2 for channel 2
                            break;

        case ADC_PIN_0_26:  LPC_PINCON->PINSEL1 |= (1<<20); // set p0.26 as adc3
                            LPC_PINCON->PINMODE1 |= (2<<20); // set NO PULL UP OR PULL DOWN REST
                            LPC_ADC->ADCR |= (1<<3); // set bit no. 3 for channel 3
                            break;

        case ADC_PIN_1_30:  LPC_PINCON->PINSEL3 |= (3<<28); // set p1.30 as adc4
                            LPC_PINCON->PINMODE3 |= (2<<28); // set NO PULL UP OR PULL DOWN REST
                            LPC_ADC->ADCR |= (1<<4); // set bit no. 4 for channel 4
                            break;

        case ADC_PIN_1_31:  LPC_PINCON->PINSEL3 |= (3<<30); // set p1.31 as adc5
                            LPC_PINCON->PINMODE3 |= (2<<30); // set NO PULL UP OR PULL DOWN REST
                            LPC_ADC->ADCR |= (1<<5); // set bit no. 5 for channel 5
                            break;
    }
    LPC_ADC->ADCR |= (1<<24); //start 001 - start conversion now.
}

float ADCDriver::readADCVoltageByChannel(uint8_t adc_channel_arg)
{
    uint32_t data;
    switch(adc_channel_arg)
    {
        case 2:  data = (LPC_ADC->ADDR2 >> 4) & (uint16_t)0x0FFF;
                 break;
        case 3:  data = (LPC_ADC->ADDR3 >> 4) & (uint16_t)0x0FFF;
                 break;
        case 4:  data = (LPC_ADC->ADDR4 >> 4) & (uint16_t)0x0FFF;
                 break;
        case 5:  data = (LPC_ADC->ADDR5 >> 4) & (uint16_t)0x0FFF;
                 break;
    }
    return data;
}
