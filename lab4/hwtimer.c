#include "hwtimer.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void InitTimer() {

    CS_setDCOFrequency(SYSTEMCLOCK);

    WDT_A_hold(WDT_A_BASE);

    // TIMER32_0_BASE is set at 8KHz period
    // When interrupts on this timer are enabled,
    // we get an ISR that executes at 8KHz
    Timer32_initModule(TIMER32_0_BASE,
                       TIMER32_PRESCALER_1,
                       TIMER32_32BIT,
                       TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, (int) (SYSTEMCLOCK / 8000));
    Timer32_startTimer(TIMER32_0_BASE, false);

    // TIMER32_0_BASE is set in continuous mode
    // It can be used to program software timers.
    Timer32_initModule(TIMER32_1_BASE,
                       TIMER32_PRESCALER_1,
                       TIMER32_32BIT,
                       TIMER32_FREE_RUN_MODE);
    Timer32_setCount(TIMER32_1_BASE, UINT32_MAX);
    Timer32_startTimer(TIMER32_1_BASE, false);
}

