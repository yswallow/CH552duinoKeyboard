#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USBHIDKeyboardMouse.h"
#include "../../KeyboardConfig.h"

volatile __xdata uint8_t UpPoint1_Busy  = 0;   //Flag of whether upload pointer is busy

__xdata uint8_t HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
__xdata uint8_t HIDMouse[4] = {0x0,0x0,0x0,0x0};

__xdata uint8_t statusLED = 0;

typedef void( *pTaskFn)( void );

void delayMicroseconds(uint16_t us);

void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared	  
    UEP2_T_LEN = 0;                                                          
}


void USB_EP1_IN(){
    UEP1_T_LEN = 0;
    UEP1_CTRL = (UEP1_CTRL & (~MASK_UEP_T_RES)) | UEP_T_RES_ACK;           // Default NAK
    UpPoint1_Busy = 0;                                                  //Clear busy flag
}


void USB_EP1_OUT(){
    if ( U_TOG_OK )                                                     // Discard unsynchronized packets
    {
        switch(Ep1Buffer[0]) {
            case 1:
                statusLED = Ep1Buffer[1];
#ifdef STATUS_LED_FUNCTION
                STATUS_LED_FUNCTION(statusLED);
#endif
                break;
            case 8:
                raw_hid_receive();
                break;
            default:
                break;
        }
    }
}

    
uint8_t USB_EP1_send(uint8_t reportID){
    uint16_t waitWriteCount = 0;
    
    waitWriteCount = 0;
    while (UpPoint1_Busy){//wait for 250ms or give up
        waitWriteCount++;
        delayMicroseconds(5);
        if (waitWriteCount>=50000) return 0;
    }
    
    if (reportID == 1){
        Ep1Buffer[64+0] = 1;
        for (uint8_t i=0;i<sizeof(HIDKey);i++){                                  //load data for upload
            Ep1Buffer[64+1+i] = HIDKey[i];
        }
        UEP1_T_LEN = 1+sizeof(HIDKey);                                             //data length
    }else if (reportID == 2){
        Ep1Buffer[64+0] = 2;
        for (uint8_t i=0;i<sizeof(HIDMouse);i++){                                  //load data for upload
            Ep1Buffer[64+1+i] = ((uint8_t *)HIDMouse)[i];
        }
        UEP1_T_LEN = 1+sizeof(HIDMouse);                                             //data length
    }
    else if (reportID == 8){
        Ep1Buffer[64+0] = 8;
        UEP1_T_LEN = 33;
    }
    
    UpPoint1_Busy = 1;
    UEP1_CTRL = (UEP1_CTRL & (~MASK_UEP_T_RES)) | UEP_T_RES_ACK;                //upload data and respond ACK
    
    return 1;
}


void raw_hid_send(void) {
    USB_EP1_send(8);
}


void send_keyboard_report(void) {
    USB_EP1_send(1);
}

void Keyboard_press(uint8_t kc)
{
	if( kc<0xE0 )
	{
	    for(uint8_t i=2;i<8;i++)
	    {
	        if( HIDKey[i]==0 ) {
	            HIDKey[i] = kc;
	            break;
	        } else if( HIDKey[i]==kc ) {
	            break;
	        }
	    }
	} else {
	    HIDKey[0] |= 1<<(kc-0xE0);
	}
}

void Keyboard_release(uint8_t kc)
{
    uint8_t backwardCount = 0;
	if( kc<0xE0 )
	{
	    for(uint8_t i=2;i<8;i++)
	    {
	        if( HIDKey[i]==kc )
	        {
	            backwardCount++;
	        }
	        
	        if( backwardCount )
	        {
	            if( i+backwardCount<8 )
	            {
	                HIDKey[i] = HIDKey[i+backwardCount];
	            } else {
	                HIDKey[i] = 0;
	            }
	        }
        }
    } else {
        HIDKey[0] &= ~(1<<(kc-0xE0));
    }
}

void Keyboard_releaseAll(void){
    for (uint8_t i=0;i<sizeof(HIDKey);i++){                                  //load data for upload
        HIDKey[i] = 0;
    }
	USB_EP1_send(1);
}

uint8_t Mouse_press(uint8_t k) {
    HIDMouse[0] |= k;
    USB_EP1_send(2);
    return 1;
}

uint8_t Mouse_release(uint8_t k) {
    HIDMouse[0] &= ~k;
    USB_EP1_send(2);
    return 1;
}

uint8_t Mouse_click(uint8_t k){
    Mouse_press(k);
    delayMicroseconds(10000);
    Mouse_release(k);
    return 1;
}

uint8_t Mouse_move(int8_t x, int8_t y){
    HIDMouse[1] = x;
    HIDMouse[2] = y;
    USB_EP1_send(2);
    return 1;
}

uint8_t Mouse_scroll(int8_t tilt){
    HIDMouse[3] = tilt;
    USB_EP1_send(2);
    return 1;
}
