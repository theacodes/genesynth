#include <SDL2/SDL.h>
#include "hardware_constants.h"
#include "Arduino.h"
#include "ym2612core/ym2612.h"

unsigned long millis() {
    return SDL_GetTicks();
}

unsigned long micros() {
    return SDL_GetTicks() * 1000;
}

uint8_t ym_data_port_byte = 0;
uint8_t ym_write_bit = 0;
uint8_t ym_write_address = 0;

void digitalWriteFast(int pin, bool value) {
    if(pin >= YM_DATA - 8 && pin <= YM_DATA) {
        auto offset = YM_DATA - pin;
        if(value) {
            ym_data_port_byte |= 1 << offset;
        } else {
            ym_data_port_byte &= ~(1 << offset);
        }
    }

    if(pin == YM_A0) {
        if(value) {
            ym_write_address |= 1;
        } else {
            ym_write_address &= ~(1);
        }
    }

    if(pin == YM_A1) {
        if(value) {
            ym_write_address |= 1 << 1;
        } else {
            ym_write_address &= ~(1 << 1);
        }
    }


    if(pin == YM_WR) {
        // Write was just activated, signaling we should send the byte to
        // the chip.
        if(!value && ym_write_bit) {
            printf("Writing %02x to port %i\n", ym_data_port_byte, ym_write_address);
            YM2612Write(ym_write_address, ym_data_port_byte);
        }
        ym_write_bit = value;
    }
}

SerialStub Serial;
