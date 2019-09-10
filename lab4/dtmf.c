#include "dtmf.h"
#include "display.h"

// TO BE DEFINED BY YOU
//
// Compute Goertzel Filter Coefficients for
// Frequency Detectors for the following frequencies:
//
//      Gtap structure     Fgoertzel
//      -----------------------------
//      t697                 697 Hz
//      t770                 770 Hz
//      t852                 852 Hz
//      t1209               1209 Hz
//      t1336               1336 Hz
//      t1477               1477 Hz
//
// A Goertzel filter coefficient is defined by
//
//           c = 2*cos(2*pi*v)
//
// with v equal to Fgoertzel / Fsample.
// For example, if Fgoertzel = 1000Hz and Fsample = 8000Hz
// then c = 2 * cos (2*pi*1/8) = 1.4142
// Be sure to compute cos() in radians
//
// This results in the following filter coefficients
//
//      Gtap structure     Fgoertzel       Coefficient
//      ----------------------------------------------
//      t697                 697 Hz      0.8538689 * 2
//      t770                 770 Hz      0.8226405 * 2
//      t852                 852 Hz      0.7843435 * 2
//      t1209               1209 Hz      0.5820520 * 2
//      t1336               1336 Hz      0.4981851 * 2
//      t1477               1477 Hz      0.3993092 * 2
//
// To encode the Goertzel filter coefficient into Gtap,
// you have to convert the floating point value of c
// into a fixed point value.
//
// For a fixed point representation <WLEN,FRACBITS>,
// The resulting Gtap coef value is
//
//         coef =  (int) (c * (1 << FRACBITS))
//
// Make sure that you do NOT use double or float in your program.
// Use ONLY integer types (signed or unsigned).

// FILL IN THE FOLLOWING VALUES TO DEFINE PROPER COEF VALUES

//#define   t1 = (int)(1.7077378*(1 << FRACBITS));
//#define   t2 = (int)(1.645281*(1 << FRACBITS));
//#define   t3 = (int)(1.568687*(1 << FRACBITS));
//#define   t4 = (int)(1.164104*(1 << FRACBITS));
//#define   t5 = (int)(0.9963702*(1 << FRACBITS));
//#define   t6 = (int)(0.7986184*(1 << FRACBITS));
Graphics_Context g_sContext;


static Gtap  t697 = { (int)(0.8538689*2*(1 << FRACBITS)) , 0, 0};
static Gtap  t770 = { (int)(0.8226405*2*(1 << FRACBITS)) , 0, 0};
static Gtap  t852 = { (int)(0.7843435*2*(1 << FRACBITS)) , 0, 0};
static Gtap t1209 = { (int)(0.5820520*2*(1 << FRACBITS)) , 0, 0};
static Gtap t1336 = { (int)(0.4981851*2*(1 << FRACBITS)), 0, 0};
static Gtap t1477 = { (int)(0.3993092*2*(1 << FRACBITS)) , 0, 0};

void DTMFAddSample(unsigned x) {
    unsigned vx = ScaleSample(x);

    SampleGoertzel( &t697, vx);
    SampleGoertzel( &t770, vx);
    SampleGoertzel( &t852, vx);
    SampleGoertzel(&t1209, vx);
    SampleGoertzel(&t1336, vx);
    SampleGoertzel(&t1477, vx);
}

void DTMFReset() {
    ResetGoertzel( &t697);
    ResetGoertzel( &t770);
    ResetGoertzel( &t852);
    ResetGoertzel(&t1209);
    ResetGoertzel(&t1336);
    ResetGoertzel(&t1477);
}

void DrawUnsigned(unsigned line, char prefix[4], unsigned n) {
    char buf[13]    = "PPPP XXXXXXXX";
    char hexstr[16] = "0123456789ABCDEF";
    buf[0]  = prefix[0];
    buf[1]  = prefix[1];
    buf[2]  = prefix[2];
    buf[3]  = prefix[3];
    buf[4]  = ' ';
    buf[5]  = hexstr[(n >> 28) & 0xF];
    buf[6]  = hexstr[(n >> 24) & 0xF];
    buf[7]  = hexstr[(n >> 20) & 0xF];
    buf[8]  = hexstr[(n >> 16) & 0xF];
    buf[9]  = hexstr[(n >> 12) & 0xF];
    buf[10] = hexstr[(n >>  8) & 0xF];
    buf[11] = hexstr[(n >>  4) & 0xF];
    buf[12] = hexstr[(      n) & 0xF];
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf,
                                             13, 64,
                                             16 + (line - 1) * 16,
                                             OPAQUE_TEXT);
}

int DTMFDisplay() {
    unsigned p697  = PowerGoertzel(  &t697);
    unsigned p770  = PowerGoertzel(  &t770);
    unsigned p852  = PowerGoertzel(  &t852);
    unsigned p1209 = PowerGoertzel( &t1209);
    unsigned p1336 = PowerGoertzel( &t1336);
    unsigned p1477 = PowerGoertzel( &t1477);

  /*  DrawUnsigned(1, " 697",  p697);
    DrawUnsigned(2, " 770",  p770);
    DrawUnsigned(3, " 852",  p852);
    DrawUnsigned(4, "1209", p1209);
    DrawUnsigned(5, "1336", p1336);
    DrawUnsigned(6, "1477", p1477);*/
    if(p697 > 200 && p1209 > 200)
        return 1;
    else if(p1336 > 200 && p697 > 200)
        return 2;
    else if(p697 > 200 && p1477 > 200)
        return 3;
    else if(p770 > 200 && p1209 > 200)
        return 4;
    else if(p770 > 200 && p1336 > 200)
        return 5;
    else if(p770 > 200 && p1477 > 200)
        return 6;
    else if(p852 > 200 && p1209 > 200)
        return 7;
    else if(p852 > 200 && p1336 > 200)
        return 8;
    else if(p852 > 200 && p1477 > 200)
        return 9;
    return 0;
}

