#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"

// Global parameters with current application settings

typedef enum {black, red, green, yellow, blue, magenta, cyan, white} color_t;
typedef enum {baud9600, baud19200, baud38400, baud57600} UARTBaudRate_t;
typedef enum {falsee, truee} booll;

typedef struct Count
{
   int CountChar;
   int CountRow;
} Count;
//Global Varibale
char FArray[3] = {'F','G', '7'}; //Foreground Color array
char BArray[3] = {'B', 'G', '4'}; //BackGroud Color array
char BdArray[7] = {'B', 'D' , ' ' , '9' , '6' , '0', '0'}; //baudRate Array

int digit1; //unit of the total count
int digit2; //tenth of the total count
int digit3; //hundred of the total count
int digit4; //thousand of the total count
int TotalCount = 0; //Total characters

//-----------------------------------------------------------------------
// Character Graphics API
//
// The 128*128 pixel screen is partitioned in a grid of 8 rows of 16 characters
// Each character is a plotted in a rectangle of 8 pixels (wide) by 16 pixels (high)
//
// The lower-level graphics functions are taken from the Texas Instruments Graphics Library
//
//            C Application        (this file)
//                   |
//                 GRLIB           (graphics library)
//                   |
//             CrystalFontz Driver (this project, LcdDriver directory)
//                   |
//                font data        (this project, fonts directory)

Graphics_Context g_sContext;

void InitGraphics() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    GrContextFontSet(&g_sContext, &g_sFontCmtt16);
    Graphics_clearDisplay(&g_sContext);
}

void LCDClearDisplay() {
    Graphics_clearDisplay(&g_sContext);
}

void LCDDrawChar(unsigned row, unsigned col, int8_t c) {
    Graphics_drawString(&g_sContext,
                        &c,
                        1,
                        8 * (col % 16),
                        16 * (row % 8),
                        OPAQUE_TEXT);
}

//------------------------------------------
// UART API
//
// We are using EUSCI_A0, which is a user UART in MSP432 that is accessible
// on your laptop as 'XDS 110 Class Application/User UART'. It usually shows up as COM3.
//
// The low-level UART functions are taken from the MSP432 Driverlib, Chapter 24.
//
// The Baud Rate Computation Procedure is taken from the
// User Guide MSP432P4 Microcontroller, Chapter 24, page 915
//
//  Baud rate computation:
//  - System Clock SMCLK in MSP432P4 is 3MHz (Default)
//  - Baud Rate Division Factor N = 3MHz / Baudrate
//      Eg. N9600 = 30000000 / 9600 = 312.5
//  - If N>16 -> oversampling mode
//      Baud Rate Divider              UCBF  = FLOOR(N/16)     = INT(312.5/16)     = 19
//      First modulation stage select  UCBRF = FRAC(N/16) * 16 = FRAC(312.5/16)*16 = 8
//      Second modulation state select UCBRS = (table 24-4 based on FRAC(312.5))   = 0xAA

eUSCI_UART_Config uartConfig = {
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 3MHz
     19,                                           // UCBR   = 19
     8,                                            // UCBRF  = 8
     0xAA,                                         // UCBRS  = 0xAA
     EUSCI_A_UART_NO_PARITY,                       // No Parity
     EUSCI_A_UART_LSB_FIRST,                       // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
     EUSCI_A_UART_MODE,                            // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
};

void InitUART() {
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
}

bool UARTHasChar() {
    return (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}

uint8_t UARTGetChar() {
    if (UARTHasChar())
        return UART_receiveData(EUSCI_A0_BASE);
    else
        return 0;
}

bool UARTCanSend() {
    return (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
}

void UARTPutChar(uint8_t t) {
    while (!UARTCanSend()) ;
    UART_transmitData(EUSCI_A0_BASE,t);
}
//The following functions change the baud rate of the UART
void changeUART19200(){
    eUSCI_UART_Config uartConfig = {
                EUSCI_A_UART_CLOCKSOURCE_SMCLK,
                9,
                12,
                0xDD,
                EUSCI_A_UART_NO_PARITY,
                EUSCI_A_UART_LSB_FIRST,                       // LSB First
                EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
                EUSCI_A_UART_MODE,                            // UART mode
                EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
           };
           UART_initModule(EUSCI_A0_BASE, &uartConfig);
           UART_enableModule(EUSCI_A0_BASE);
} //change the baud rate of the UART to 19200

void changeUART38400(){
    eUSCI_UART_Config uartConfig = {
                EUSCI_A_UART_CLOCKSOURCE_SMCLK,
                4,
                14,
                0x10,
                EUSCI_A_UART_NO_PARITY,
                EUSCI_A_UART_LSB_FIRST,                       // LSB First
                EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
                EUSCI_A_UART_MODE,                            // UART mode
                EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
           };
            UART_initModule(EUSCI_A0_BASE, &uartConfig);
            UART_enableModule(EUSCI_A0_BASE);
} //change the baud rate of the UART to 38400

void changeUART57600(){
    eUSCI_UART_Config uartConfig = {
                EUSCI_A_UART_CLOCKSOURCE_SMCLK,
                3,
                4,
                0xBF,
                EUSCI_A_UART_NO_PARITY,
                EUSCI_A_UART_LSB_FIRST,                       // LSB First
                EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
                EUSCI_A_UART_MODE,                            // UART mode
                EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
           };
            UART_initModule(EUSCI_A0_BASE, &uartConfig);
            UART_enableModule(EUSCI_A0_BASE);
} //change the baud rate of the UART to 57600

//------------------------------------------
//Timer
void InitTimer() {

    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
    Timer32_initModule(TIMER32_0_BASE,TIMER32_PRESCALER_1,
                           TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    Timer32_initModule(TIMER32_1_BASE,TIMER32_PRESCALER_1,
                              TIMER32_32BIT,TIMER32_PERIODIC_MODE);
} // Init two timers

void TimerDebounceStartOneShot() {
    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
    Timer32_setCount(TIMER32_0_BASE, 300000);
    Timer32_startTimer(TIMER32_0_BASE, true);
    // ---
} //start the debounce timer

int TimerDebounceExpiredOneShot() {

    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
    return (Timer32_getValue(TIMER32_0_BASE) == 0);
    // ---
} //check if the debounce timer is expired

void TimerStartOneShot() {

    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
    Timer32_setCount(TIMER32_1_BASE, 666000);
    Timer32_startTimer(TIMER32_1_BASE, true);
    // ---
} //start the 222ms one shot timer

int TimerExpiredOneShot() {

    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
    return (Timer32_getValue(TIMER32_1_BASE) == 0);
    // ---
} //check if the 222ms one shot timer expired


//------------------------------------------
// Red LED API

void InitRedLED() {
    GPIO_setAsOutputPin    (GPIO_PORT_P1,    GPIO_PIN0);   // red LED on Launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P1,    GPIO_PIN0);
}

void RedLEDToggle() {
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

//Top button
void InitButtonS1() {
    GPIO_setAsInputPin (GPIO_PORT_P5, GPIO_PIN1); // upper switch S1 on BoostXL
}

int ButtonS1Pressed() {
    return (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 0);
}

//Bottom button
void InitButtonS2() {
    GPIO_setAsInputPin (GPIO_PORT_P3, GPIO_PIN5);
}

int ButtonS2Pressed() {
    return (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0);
}

//---------------------
//BounceFSM for debounce
bool BounceFSM(bool b) {

    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
        typedef enum {stable0, trans0, stable1, trans1} state_t;
        static state_t S = stable0;
        bool rval;

        switch (S) {
        case stable0:
            rval = false;
            if (b) {
                TimerDebounceStartOneShot();
                S = trans0;
            }
            break;
        case trans0:
            rval = false;
            if (b && TimerDebounceExpiredOneShot())
                S = stable1;
            if (!b)
                S = stable0;
            break;
        case stable1:
            if (!b) {
                TimerDebounceStartOneShot();
                S = trans1;
            }
            rval = true;
            break;
        case trans1:
            if (!b && TimerDebounceExpiredOneShot())
                S = stable0;
            if (b)
                S = stable1;
            rval = true;
            break;
        }
        return rval;
}

//LEDs for Extra Credit
void InitLEDS() {
    GPIO_setAsOutputPin    (GPIO_PORT_P1,    GPIO_PIN0);   // red LED on Launchpad
    GPIO_setOutputLowOnPin (GPIO_PORT_P1,    GPIO_PIN0);

    GPIO_setAsOutputPin    (GPIO_PORT_P2,    GPIO_PIN6);   // color LED red on sensor board
    GPIO_setAsOutputPin    (GPIO_PORT_P2,    GPIO_PIN4);   // color LED green on sensor board
    GPIO_setAsOutputPin    (GPIO_PORT_P5,    GPIO_PIN6);   // color LED blue on sensor board
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,    GPIO_PIN6);
    GPIO_setOutputLowOnPin (GPIO_PORT_P2,    GPIO_PIN4);
    GPIO_setOutputLowOnPin (GPIO_PORT_P5,    GPIO_PIN6);
}
//a state machine for change the color of the LED
void ColorLEDSet(color_t t) {
    switch (t) {
    case black:
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputLowOnPin (GPIO_PORT_P5, GPIO_PIN6);
        break;
    case red:
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputLowOnPin (GPIO_PORT_P5, GPIO_PIN6);
        break;
    case green:
        TimerStartOneShot();
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputLowOnPin (GPIO_PORT_P5, GPIO_PIN6);
        break;
    case yellow:
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputLowOnPin (GPIO_PORT_P5, GPIO_PIN6);
        break;
    case blue:
        TimerStartOneShot();
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
        break;
    case magenta:
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
        break;
    case cyan:
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
        break;
    case white:
        TimerStartOneShot();
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
        break;
    }
}

void ColorLEDOFF(){
           GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
           GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
           GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
}




// Change Foreground and BackGroud Color
char changeColor(char c, char ColorArray[2]){
    TotalCount = TotalCount + 3;
    switch(c){
        case '0':
            if(ColorArray[0] == 'f') {
                FArray[2] = '0'; //keep the  number in the array
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '0'; //keep the  number in the array
                Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
            }
            return '0';
        case '1':
            if(ColorArray[0] == 'f'){
                FArray[2] = '1';//keep the  number in the array
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '1';//keep the  number in the array
                Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
            }
            return '1';
        case '2':
            if(ColorArray[0] == 'f'){
                FArray[2] = '2';//keep the  number in the array
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '2';//keep the  number in the array
                Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            }
            return '2';
        case '3':
            if(ColorArray[0] == 'f'){
                FArray[2] = '3'; //keep the  number in the array
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '3'; //keep the  number in the array
                Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
            }
            return '3';
        case '4':
            if(ColorArray[0] == 'f'){
                FArray[2] = '4'; //keep the  number in the array
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '4'; //keep the  number in the array
                 Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
            }
            return '4';
        case '5':
            if(ColorArray[0] == 'f'){
                FArray[2] = '5'; //keep the  number in the array
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_MAGENTA);
            }
            else if(ColorArray[0] == 'b'){
                  BArray[2] = '5'; //keep the  number in the array
                  Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_MAGENTA);
            }
            return '5';
        case '6':
            if(ColorArray[0] == 'f'){
                FArray[2] = '6'; //keep the  number in the array
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_CYAN);
            }
            else if(ColorArray[0] == 'b'){
                BArray[2] = '6'; //keep the  number in the array
                 Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_CYAN);
            }
            return '6';
        case '7':
            if(ColorArray[0] == 'f'){
                FArray[2] = '7'; //keep the  number in the array
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
            }
            else if(ColorArray[0] == 'b'){
                 BArray[2] = '7'; //keep the  number in the array
                 Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
            }
            return '7';
        default:
             return 'n';
    }
}

//Count Characters and Rows and Print on Laptop and LCD
Count countAndPrint(Count count,char c){
   count.CountChar++;
   TotalCount++;
   if (count.CountChar % 16 == 0)
   {
       count.CountRow++;
   }
   UARTPutChar(c);
   LCDDrawChar(count.CountRow, count.CountChar, c);
   return count;
}

//count the characters and if the letters after # is not right
Count SkipPrint(Count count, char c){
    TotalCount = TotalCount + 2;
    count.CountChar++;
    if (count.CountChar % 16 == 0)
    {
       // count.CountChar = 0;
        count.CountRow++;
    }
    LCDDrawChar(count.CountRow, count.CountChar, '#');

    count.CountChar++;
     if (count.CountChar == 0)
     {
         count.CountChar = 0;
         count.CountRow++;
     }
     LCDDrawChar(count.CountRow, count.CountChar, c);
     UARTPutChar('#');
     UARTPutChar(c);
     return count;
}


//Top  Button Print Message
Count printStatus(int countChar){
    LCDClearDisplay();
    digit1 = TotalCount/1 % 10 + '0'; //unit
    digit2 = TotalCount/ 10 % 10 + '0'; //ten
    digit3 = TotalCount/100 % 10 + '0'; //hundred
    digit4 = TotalCount/1000 % 10 + '0'; //thousand
    int i = 0;
    for(i = 0; i < 7; i++)
    {
        LCDDrawChar(0,i, BdArray[i]);
        UARTPutChar(BdArray[i]);
    } //print the baud rate information for the status message

    LCDDrawChar(0, 8 ,' ');
    UARTPutChar(' ');
    LCDDrawChar(0, 9, FArray[0]);
    UARTPutChar(FArray[0]);
    LCDDrawChar(0, 10,FArray[1]);
    UARTPutChar(FArray[1]);
    LCDDrawChar(0, 11,FArray[2]);
    UARTPutChar(FArray[2]);
    LCDDrawChar(0, 12, ' ');
    UARTPutChar(' '); //print the foreground color information on the LCD

    LCDDrawChar(0, 13, BArray[0]);
    UARTPutChar(BArray[0]);
    LCDDrawChar(0, 14, BArray[1]);
    UARTPutChar(BArray[1]);
    LCDDrawChar(0, 15, BArray[2]);
    UARTPutChar(BArray[2]);
    UARTPutChar(' ');
    UARTPutChar('N');
    LCDDrawChar(1,0,'N');
    //UARTPutChar(' ');
    LCDDrawChar(1,1, ' '); //print the background color information on the LCD

    LCDDrawChar(1,2,digit4);
    UARTPutChar(digit4);
    LCDDrawChar(1,3,digit3);
    UARTPutChar(digit3);
    LCDDrawChar(1,4,digit2);
    UARTPutChar(digit2);
    LCDDrawChar(1,5,digit1);
    UARTPutChar(digit1); //print the total count of the characters on the LCD


    Count count;
    count.CountChar =-1;
    count.CountRow = 1;
    return count; //return the struct
}

//Bottom Button, Change Baudrate
UARTBaudRate_t ChangeBaud(){
      static UARTBaudRate_t t = baud9600;
    // --- YOU HAVE TO WRITE THIS FUNCTION BODY
       switch(t) {
       case baud9600:
           changeUART19200();
           BdArray[0] = 'B';
           BdArray[1] = 'D';
           BdArray[2] = '1';
           BdArray[3] = '9';
           BdArray[4] = '2';
           BdArray[5] = '0';
           BdArray[6] = '0';
           t = baud19200;
           return t;
           break;
       case baud19200:
           changeUART38400();
           BdArray[0] = 'B';
           BdArray[1] = 'D';
           BdArray[2] = '3';
           BdArray[3] = '8';
           BdArray[4] = '4';
           BdArray[5] = '0';
           BdArray[6] = '0';
            t = baud38400;
           return t;
       case baud38400:
           changeUART57600();
           BdArray[0] = 'B';
           BdArray[1] = 'D';
           BdArray[2] = '5';
           BdArray[3] = '7';
           BdArray[4] = '6';
           BdArray[5] = '0';
           BdArray[6] = '0';
           t = baud57600;
           return t;
           break;
       case baud57600:
           UART_initModule(EUSCI_A0_BASE, &uartConfig);
           UART_enableModule(EUSCI_A0_BASE);
           BdArray[0] = 'B';
           BdArray[1] = 'D';
           BdArray[2] = ' ';
           BdArray[3] = '9';
           BdArray[4] = '6';
           BdArray[5] = '0';
           BdArray[6] = '0';
           t = baud9600;
           return t;
       }
}

//Extra credit
void ExtraCredit(char c){
   if(c == '#')
   {
        ColorLEDSet(white);
        while(!TimerExpiredOneShot()){ }
        ColorLEDOFF();
   } //if the character is #
   else if(c <= '9' && c >= '0')
   {
        TimerStartOneShot();
        ColorLEDSet(red);
        while(!TimerExpiredOneShot()){}
        ColorLEDOFF();
   } //if the character is a number
   else if( (c <= 'z' && c>= 'a') || (c <= 'Z' && c >= 'A'))
   {
      ColorLEDSet(blue);
       while(!TimerExpiredOneShot()){}
       ColorLEDOFF();
   } //if the character is letters
   else
   {
        ColorLEDSet(green);
         while(!TimerExpiredOneShot()){}
         ColorLEDOFF();
   }
}

#define UPEDGE(A, B) (A && !(B))

int main(void) {

    uint8_t c;
    WDT_A_hold(WDT_A_BASE);
    InitLEDS();
    InitGraphics();
    InitUART();
    InitRedLED();

    //Initialize all variable
   // static UARTBaudRate_t baud = baud9600;
    Count count;
    count.CountChar= -1; //count chars
    count.CountRow = -1; //count rows
    char ColorArray[2] = {}; //keep characters after #
    int countColorArray = 0, command = -1;
    booll changeColorNext = falsee;
    booll skipPrint = falsee;
    booll first = truee;
    booll inLine = falsee;
    bool S2d = false, prev_S2d;

    InitTimer();
    InitButtonS1();
    InitButtonS2();
    InitRedLED();

    while (1) {
        if (UARTHasChar()) {

            if(command == 0){
                command = -1;
            }
            c = UARTGetChar();
            ExtraCredit(c); //call the Extra Credit function
           // RedLEDToggle();

            if(c == '#' && first == truee){
                first  = falsee;
                inLine = truee;
                skipPrint = truee;
            } //if there is '#'
            else if(inLine == truee && changeColorNext == falsee){
               if(c == 'f' || c == 'b'){
                   ColorArray[countColorArray] = c;
                   changeColorNext = truee;
               }
               else skipPrint = falsee;
            } //if last character is '#'

            else if(changeColorNext == truee){
                changeColor(c, ColorArray);
                countColorArray = 0;
                changeColorNext = falsee;
                inLine = falsee;
                skipPrint = falsee;
                command = 0;
            } //if last two characters are # and f or b

            if(skipPrint == falsee )
            {
               if(inLine == truee) {
                   count = SkipPrint(count, c);
                   inLine = falsee;
                   first = truee;
                  }
                else if(command == -1) {
                   count = countAndPrint(count, c);
                   first = truee;
                }
            } //print and count the letter if the letter after # is not right
        }
           //Top button
               if (ButtonS1Pressed()) {
                    count = printStatus(count.CountChar);
                     }
           //Bottom Button
               prev_S2d = S2d;
               S2d = BounceFSM(ButtonS2Pressed());

               if (UPEDGE(S2d, prev_S2d)){
                    ChangeBaud();
               }
    }
}
