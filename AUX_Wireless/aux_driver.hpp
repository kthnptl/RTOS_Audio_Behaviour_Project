#include <sys/types.h>
#ifndef AUX_DRIVER_HPP_
#define AUX_DRIVER_HPP_

class AUX{
private:

public:
    void aux_init();
    void aux_select_pins();
    void aux_read(uint8_t *Freq_value);
    void aux_adc_read();
    void aux_print(uint8_t *Freq_value);
    void set_data(uint8_t index, uint8_t Freq_value);
    uint8_t get_data(uint8_t index);
    typedef union data_t{
        struct data_struct{
            uint8_t band:       2;
            /*
             * 00: for strip #1 -> either 63Hz  or  160Hz,
             * 01: for strip #2 -> either 400Hz or  1kHz,
             * 10: for strip #3 -> either 2.5kHz, 6.25kHz
             * 11: for strip #4 -> for    16kHz
             * */
            uint8_t speed:      2;
            /*
             * determines the speed of LED blinks
             * 00: slow
             * 11: fast
             * */
            uint8_t intensity:  4;
            /*
             * shows the % in the range from 0-7
             * */
        }data_bits;
        uint8_t data_byte;
    }data;
};

#endif /* AUX_DRIVER_HPP_ */
