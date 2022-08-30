#include <Arduino.h>
#include "./via.h"
#include "./USBHIDKeyboardMouse.h"
#include "../../KeyboardConfig.h"

uint8_t press_status[ROWS_COUNT];

__code uint8_t rows[ROWS_COUNT] = MATRIX_ROWS;
__code uint8_t cols[COLS_COUNT] = MATRIX_COLS;

void press_key(uint8_t row, uint8_t col, bool press)
{
    uint16_t keycode = dynamic_keymap_get_keycode(0,row,col);
    uint8_t application = (keycode>>8) & 0x00FF;
    
    if( application )
    {
        switch( application & 0xF0 )
        {
            case 0x00:
                for(uint8_t i=0;i<4;i++)
                {
                    if( application & (1<<i) )
                    {
                        if( press )
                        {
                            Keyboard_press( 0xE0+i );
                        } else {
                            Keyboard_release( 0xE0+i );
                        }
                    }
                }
                break;
            case 0x10:
                for(uint8_t i=0;i<4;i++)
                {
                    if( application & (1<<i) )
                    {
                        if( press )
                        {
                            Keyboard_press( 0xE4+i );
                        } else {
                            Keyboard_release( 0xE4+i );
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    
    if( press )
    {
        Keyboard_press(keycode & 0x00FF);
    } else {
        Keyboard_release(keycode & 0x00FF);
    }
}

void keyboard_init(void)
{
    USBInit();
    
    for(uint8_t i=0; i<ROWS_COUNT; i++) {
        press_status[i] = 0;
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], LOW);
    }
    
    for(uint8_t i=0; i<COLS_COUNT; i++) {
        pinMode(cols[i], INPUT_PULLUP);
    }   
}

void keyboard_scan(void)
{
    bool matrix_change = false;
    
    for(uint8_t row=0;row<ROWS_COUNT;row++)
    {
        digitalWrite(rows[row], LOW);
        for(uint8_t col=0;col<COLS_COUNT;col++)
        {
            if(digitalRead(cols[col])==LOW)
            {
                // press
                if(! (press_status[row] & (1<<col)) )
                {
                    press_status[row] |= 1<<col;
                    press_key(row,col,true);
                    matrix_change = true;
                }
            } else {
                // release
                if( press_status[row] & (1<<col) )
                {
                    press_status[row] &= ~(1<<col);
                    press_key(row,col,false);
                    matrix_change = true;
                }
            }
        }
        digitalWrite(rows[row], HIGH);
    }
    
    if( matrix_change )
    {
        send_keyboard_report();
    }
}
