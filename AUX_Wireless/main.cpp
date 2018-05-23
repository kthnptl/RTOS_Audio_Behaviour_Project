#include<stdio.h>
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"
#include "utilities.h"
#include "io.hpp"
#include "printf_lib.h"
#include "adc_driver.h"
#include "aux_driver.hpp"
#include "semphr.h"

#define STACK_SIZE 2048
uint8_t voltage[7] = {0};
AUX aux;
SemaphoreHandle_t sema;

void nordic_send(void * par)
{
    char addr = 109;
    uint8_t index = 0;
    mesh_packet_t tx_pkt;
    uint8_t data = 0;
    while(1)
    {
        if(xSemaphoreTake(sema, portMAX_DELAY))
        {
            for(index = 0; index < 7 ; index++)
            {
                data = voltage[index];
                mesh_form_pkt(&tx_pkt, addr, mesh_pkt_nack, 1, 1, &data, sizeof(uint8_t));
                mesh_send_formed_pkt(&tx_pkt);
            }
        }
    }
}

void readAdcData( void * pvParameters )
{
    while(1){
        aux.aux_read(voltage);
        if(! xSemaphoreGive(sema)){
            u0_dbg_printf("unable to give \n");
        }
        vTaskDelay(20);
    }
}

#if 1
int main(void){
    sema = xSemaphoreCreateBinary();
    aux.aux_select_pins();
    aux.aux_init();
    aux.aux_adc_read();
    if( ! wireless_init())
    {
        printf("Wireless Init Unsuccessfull...\n");

    }
    xTaskCreate(readAdcData,"readAdc",STACK_SIZE, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(nordic_send, "wireless send", STACK_SIZE, NULL, PRIORITY_MEDIUM, NULL);
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));
    scheduler_start(); ///< This shouldn't return
    return -1;
}
#endif

