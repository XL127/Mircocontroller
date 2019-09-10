#include "sound.h"
#include "hwtimer.h"
#include "swtimer.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// This configuration sets a 50% duty cycle configuration
// for CCR4. You will have to figure out what Timer_A module
// drives the buzzer, i.e. what pin TAx.4 is driving the
// buzzer. The Timer_A module then is x. double frequency


double time = 392;



void InitSound() {

    // This function switches the IC pin connected to
    // the buzzer from GPIO functionality to Timer_A functionality
    // so that we can drive it with PWM.

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P2,
            GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);
}

void PlaySound(tnote n, unsigned ms) {

    switch(n) {
    case note_g4:
       time = 392;
    break;
    case note_c5:
       time = 523.25;
    break;
    case note_e5:
       time = 659.25;
    break;
    case note_a4:
        time = 440;
    break;
    case note_b4:
        time = 493.88;
    break;
    case note_d5:
        time = 587.33;
    break;
    case note_f5:
        time = 698.46;
    break;
    case note_fs5:
        time = 739.99;
    break;
    case note_g5:
        time = 783.99;
    break;
    case note_c6:
        time = 1046.5;
    break;
    case note_silent:
        time = 1;
    break;
    case note_a5:
        time = 880;
    break;
    case note_b5:
        time = 987.77;
    break;
    }

    Timer_A_PWMConfig pwmConfig = {
               TIMER_A_CLOCKSOURCE_SMCLK,
               TIMER_A_CLOCKSOURCE_DIVIDER_1,
               (int) (SYSTEMCLOCK / time),         // C5
               TIMER_A_CAPTURECOMPARE_REGISTER_4,
               TIMER_A_OUTPUTMODE_RESET_SET,
               (int) ((SYSTEMCLOCK/2) / time)
       };

    static tSWTimer OST;

    //StartOneShotSWTimer(&OST);
    //static OneShotSWTimer_t OST;
  //  InitOneShotSWTimer(&OST,TIMER32_1_BASE,ms);
    //=============================================================
    // TO BE COMPLETED BY YOU

    // Play note n for ms milliseconds.

    // You have to use the PWM setting of the Timer_A
    // peripheral that drives the buzzer to sound it

    // The delay ms is generated using a software timer
    // (different from Timer_A!)

    // PlaySound can be implemented as a blocking function.
    // That means that the function turns on the PWM
    // generation, then waits for ms milliseconds, then
    // turns the PWM generation off again.

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    InitSWTimer(&OST,TIMER32_1_BASE, ms*10000);
    StartSWTimer(&OST);
    while(1) {
     if (SWTimerOneShotExpired(&OST)) {
         Timer_A_stopTimer(TIMER_A0_BASE);
         return;
       }
   }

    //=============================================================

}
