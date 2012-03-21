// Simple analog to digitial conversion

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include "analog.h"

#include "Keyboard.h"

static uint8_t aref = (1<<REFS0); // default to AREF = Vcc

static int16_t ref_x, ref_y, max_x, min_x, max_y, min_y;

#define DELTAMAX 12 ///< maximum return value: X/Y are scaled to [-delta .. +delta]
#define DEADZONE 0.0002 ///< movements within this part of the range (extreme-reference) are not reported

void Analog_Init(void)
{
    ref_x=adc_read(0);
    ref_y=adc_read(1);

    // set some sane defaults so mouse does not move on startup
    max_x=max_y=0x039A;
    min_x=min_y=0x0025;

}

/*    min..........ref..............max
 *               | DZ  |
 */
int16_t delta(int16_t val, int16_t ref, int16_t limit)
{
    int16_t accel = 50*(val-ref)/(limit-ref); // 0..100
    if(accel < 0)
        accel *= -1;
    if(abs(val-ref) > (limit-ref)*DEADZONE)
        return(-DELTAMAX*(val-ref*(1-DEADZONE))/abs(limit-ref) *(30+accel)/100);
    return 0;

}

void getXY( int16_t *dx, int16_t *dy)
{
    int16_t x= adc_read(0);
    int16_t y= adc_read(1);

    if(x==0xFFFF || y== 0xFFFF) {
        *dx=0;
        *dy=0;
        return;
    }

    if(x > max_x) max_x=x;
    else if(x < min_x) min_x=x;
    if(y > max_y) max_y=y;
    else if(y < min_y) min_y=y;


    *dx=  delta(x,ref_x,max_x);
    *dy= -delta(y,ref_y,max_y);
}

/*
void analogReference(uint8_t mode)
{
    aref = mode & 0xC0;
}
*/

// Mux input
int16_t adc_read(uint8_t mux)
{
    uint8_t low;

    ADCSRA = (1<<ADEN) | ADC_PRESCALER;     // enable ADC
    ADCSRB = (1<<ADHSM) | (mux & 0x20);     // high speed mode
    ADMUX = aref | (mux & 0x1F);            // configure mux input
    ADCSRA = (1<<ADEN) | ADC_PRESCALER | (1<<ADSC); // start the conversion
    while (ADCSRA & (1<<ADSC)) ;            // wait for result
    low = ADCL;                 // must read LSB first
    return (ADCH << 8) | low;           // must read MSB only once!
}


/**
  * Depending on mouse mode status either mousebutton keys are scanned,
  * or analog values mapped onto modifier keys.
  *
  * @todo move into mouse module
  */
void analogDataAcquire(void)
{
    /// @todo: Hardcoded mouse layer
    g_mouse_keys = 0;
    analogData.layer=analogData.mods=0;

    if(g_mouse_mode) {
        /*  //should be handled in key detection routine
        if(rowData[5] & (1<<1))
            g_mouse_keys = 0x01;
        if(rowData[5] & (1<<2))
            g_mouse_keys = 0x04;
        if(rowData[5] & (1<<3))
            g_mouse_keys = 0x02;
        if(rowData[4] & (1<<1))
            g_mouse_keys = 0x08;
        */
    } else {
        int16_t dx=0, dy=0;
        getXY(&dx, &dy);

        analogData.x = dx;
        analogData.y = dy;

        if(dx>1)
            analogData.mods = 0x02 ; // SHIFT
        else if(dx<-1)
            analogData.layer = 3;

        if(dy>1)
            analogData.mods = 0x01 ; // CTRL
        else if(dy<-1)
            analogData.layer = 2;

        //printf("\nMouse: %d/%d %d %d",analogData.mods,analogData.layer , dx, dy);
    }
}
