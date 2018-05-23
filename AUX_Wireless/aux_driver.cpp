#include "aux_driver.hpp"
#include <iostream>
#include "LPC17xx.h"
#include "adc_driver.h"
#include "utilities.h"
#include "printf_lib.h"
#include <sys/types.h>

using namespace std;
AUX::data d[7];

void AUX::aux_init(){
    LPC_GPIO0->FIOCLR = (1 << 0); //STROBE  CLEAR
    LPC_GPIO0->FIOSET = (1 << 1);   //RESET SET
    LPC_GPIO0->FIOSET = (1 << 0);   //STROBE    SET
    LPC_GPIO0->FIOCLR = (1 << 0);   //STROBE CLEAR
    LPC_GPIO0->FIOCLR = (1 << 1); // RESET CLEAR
    delay_us(72);
}

void AUX::aux_select_pins(){
    LPC_GPIO0->FIODIR |= (1 << 0);//strobe pin
    LPC_GPIO0->FIODIR |= (1 << 1);//reset pin
    LPC_GPIO0->FIODIR &= ~(1 << 26);//adc pin
}

void AUX::aux_read(uint8_t Freq_value[]){
    ADCDriver A1;
    uint8_t temp = 0;
    data temp_var;

    for(int band_i = 0; band_i <7; band_i++){
        delay_us(10);
        temp = ((A1.readADCVoltageByChannel((uint8_t)3) - 500)/4096)*8;
        temp_var.data_bits.band = band_i;
        if(band_i == 0)
        {
            temp_var.data_bits.band = 0;
            temp_var.data_bits.speed = 0;
            temp_var.data_bits.intensity = temp;
        }
        else if( band_i  == 2)
        {
            temp_var.data_bits.band = 1;
            temp_var.data_bits.speed = 0;
            temp_var.data_bits.intensity = temp;

        }
        else if( band_i  == 3)
        {
            temp_var.data_bits.band = 2;
            temp_var.data_bits.speed = 3;
            temp_var.data_bits.intensity = temp;
        }
        else if( band_i  == 4)
        {
            temp_var.data_bits.band = 3;
            temp_var.data_bits.speed = 3;
            temp_var.data_bits.intensity = temp;
        }
       else
       {
           temp_var.data_bits.band = 0;
           temp_var.data_bits.speed = 0;
           temp_var.data_bits.intensity = 0;
       }
        Freq_value[band_i] = temp_var.data_byte;
        delay_us(50);
        LPC_GPIO0->FIOSET = (1 << 0);
        delay_us(18);
        LPC_GPIO0->FIOCLR = (1 << 0);
    }
}

void AUX::aux_adc_read(){
    ADCDriver A1;
    A1.adcInitBurstMode();
    A1.adcSelectPin(ADCDriver::ADC_PIN_0_26);
}

void AUX::aux_print(uint8_t *Freq_value){
    int i;
    for(i=0;i<7;i++){
        //u0_dbg_printf("channel#: %d and value: %u\n", i, Freq_value[i]);
    }
    u0_dbg_printf(" \n\n");
}

void AUX::set_data(uint8_t index, uint8_t intensity){
    d[index].data_bits.band = index / 2;
    d[index].data_bits.intensity = intensity;
    if(index % 2 == 0) {
        d[index].data_bits.speed = 0;
    }
    else{
        d[index].data_bits.speed = 3;
    }
}

uint8_t AUX::get_data(uint8_t index){
    return d[index].data_byte;
}