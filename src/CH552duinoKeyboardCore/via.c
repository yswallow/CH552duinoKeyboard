#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "./USBconstant.h"
#include "./USBhandler.h"
#include "Arduino.h"
#include "USBHIDKeyboardMouse.h"

#include "../../KeyboardConfig.h"

enum {
    ID_GET_PROTOCOL_VERSION = 0x01,
    ID_GET_KEYBOARD_VALUE,
    ID_SET_KEYBOARD_VALUE,
    ID_KEYMAP_GET_KEYCODE,
    ID_KEYMAP_SET_KEYCODE,
    ID_KEYMAP_RESET,
    ID_LIGHTING_SET_VALUE,
    ID_LIGHTING_GET_VALUE,
    ID_LIGHTING_SAVE,
    ID_EEPROM_RESET,
    ID_BOOTLOADER_JUMP,
    ID_MACRO_GET_COUNT,
    ID_MACRO_GET_BUFFER_SIZE,
    ID_MACRO_GET_BUFFER,
    ID_MACRO_SET_BUFFER,
    ID_MACRO_RESET,
    ID_KEYMAP_GET_LAYER_COUNT,
    ID_KEYMAP_GET_BUFFER,
    ID_KEYMAP_SET_BUFFER,
    ID_UNHANDLED = 0xFF,
};

uint16_t dynamic_keymap_get_keycode(uint8_t layer, uint8_t row, uint8_t col) {
    uint8_t addr = (layer*ROWS_COUNT*COLS_COUNT) + row*COLS_COUNT*2 + col*2;
    return eeprom_read_byte(addr)<<8 | eeprom_read_byte(addr+1);
}

void dynamic_keymap_set_keycode(void) {
    uint8_t addr = (Ep1Buffer[2]*ROWS_COUNT*COLS_COUNT) + Ep1Buffer[3]*COLS_COUNT*2 + Ep1Buffer[4]*2;
    eeprom_write_byte(addr, Ep1Buffer[5]);
    eeprom_write_byte(addr+1, Ep1Buffer[6]);
}
    

void raw_hid_receive(void) {
    uint8_t *ResponseBuf = Ep1Buffer+65;
    uint16_t keycode;
    
    for(uint8_t i=0; i<32; i++) {
        ResponseBuf[i] = Ep1Buffer[i+1];
    }
    
    switch( Ep1Buffer[1] ) {
        case ID_GET_PROTOCOL_VERSION:
            ResponseBuf[1] = 0;
            ResponseBuf[2] = 9;
            break;
        case ID_GET_KEYBOARD_VALUE:
            for(uint8_t i=2; i<32;i++) {
                ResponseBuf[i] = 0;
            }
            break;
        case ID_SET_KEYBOARD_VALUE:
            break;
        case ID_KEYMAP_GET_KEYCODE:
            keycode = dynamic_keymap_get_keycode(Ep1Buffer[2], Ep1Buffer[3], Ep1Buffer[4]);
            ResponseBuf[4] = keycode>>8 & 0x00FF;
            ResponseBuf[5] = keycode & 0x00FF;
            break;
        case ID_KEYMAP_SET_KEYCODE:
            dynamic_keymap_set_keycode();
            break;
        case ID_MACRO_GET_COUNT:
            ResponseBuf[1] = 0;
            break;
        case ID_MACRO_GET_BUFFER_SIZE:
            ResponseBuf[2] = 0;
            ResponseBuf[3] = 0;
            break;
        case ID_MACRO_GET_BUFFER:
            break;
        case ID_MACRO_SET_BUFFER:
            break;
            
        case ID_KEYMAP_GET_LAYER_COUNT:
            ResponseBuf[1] = 1;
            break;
        case ID_KEYMAP_GET_BUFFER:
            for(uint8_t i=0;i<Ep1Buffer[4];i++) {
                ResponseBuf[4+i] = eeprom_read_byte(Ep1Buffer[3]+i);
            }
            break;
        case ID_KEYMAP_SET_BUFFER:
            for(uint8_t i=0;i<Ep1Buffer[4];i++) {
                eeprom_write_byte(Ep1Buffer[3]+i, Ep1Buffer[5+i]);
            }
            break;
        default:
            ResponseBuf[0] = ID_UNHANDLED;
            break;
    }
    delay(2);
    raw_hid_send();
}
                
