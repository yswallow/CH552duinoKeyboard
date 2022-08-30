#ifndef __USB_HID_KBD_H__
#define __USB_HID_KBD_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

enum MOUSE_BUTTON {
    MOUSE_LEFT = 1,
    MOUSE_RIGHT = 2,
    MOUSE_MIDDLE = 4,
};

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

void Keyboard_press(uint8_t kc);
void Keyboard_release(uint8_t kc);
void Keyboard_releaseAll(void);
    
uint8_t Mouse_press(uint8_t k);
uint8_t Mouse_release(uint8_t k);
uint8_t Mouse_click(uint8_t k);
uint8_t Mouse_move(int8_t x, int8_t y);
uint8_t Mouse_scroll(int8_t tilt);

void raw_hid_send(void);
void send_keyboard_report(void);

#include "USBconstant.h"

extern __xdata __at (EP0_ADDR) uint8_t  Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t  Ep1Buffer[];

#ifdef __cplusplus
} // extern "C"
#endif

#endif

