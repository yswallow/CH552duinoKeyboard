/*
  HID Keyboard mouse combo example


  created 2022
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/CH552duinoKeyboardCore/KeyboardCore.h"

void setup() {
  keyboard_init();
}

void loop() {
  keyboard_scan();
  delay(50);  //naive debouncing
}
