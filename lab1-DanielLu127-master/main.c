#include <ti/devices/msp432p4xx/driverlib/driverlib.h>


#define LEFT_BUTTON    BIT1

// THIS MACRO HAS TO BE DEFINED BY YOU
#define RIGHT_BUTTON   BIT4

#define LED1           BIT0

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_RED       BIT0

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_GREEN     BIT1

// THIS MACRO HAS TO BE DEFINED BY YOU
#define LED2_BLUE      BIT2

#define PRESSED 0 //define PRESSED as 0

#define UNPRESSED 1 //define UNPRESSED as 1

enum {RED, BLUE, GREEN, PURPLE, UNKNOW} color = RED;

void main(void) {

  WDT_A_hold(WDT_A_BASE);

  // Configure the pin connected to LED1 as output
  GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
  //COnfigure the pin connected to LED2 as output
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);


 // Configure the pin connected to left and right button
  GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);
  GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN4);

// define two status of each button and initialize to UNPRESSED
  unsigned char left_button_prev , left_button_cur ;
  unsigned char right_button_prev = UNPRESSED, right_button_cur;

  P2OUT = PRESSED; // set the all the port2's pins to 0
  P1OUT = (P1OUT & ~BIT0); //set the LED1's pin to 0

  while(1) {


      // P1IN contains all the pins on port1
      // When we mask it with LEFT_BUTTON, all the port bits become 0 except for the one representing left button
      // When this masked value becomes 0, it means this button is pressed (grounded).
      left_button_cur = (P1IN & LEFT_BUTTON);
      right_button_cur = (P1IN & RIGHT_BUTTON);

      // We recall that "pushing a button" consists of pressing and then releasing it.
      // If the button "was" pressed, but now it "is not" pressed, it means the user has finished "pushing the button"



      if ((left_button_prev == PRESSED) &&
          (left_button_cur  != PRESSED)) {
          P1OUT ^= LED1; //toggle the LED
      }

      if ((right_button_prev == PRESSED) && (right_button_cur != PRESSED))
      {
           switch(color)
           {
           case RED:
               P2OUT = LED2_RED;
              // make LED2 turning red color
               break;
           case BLUE:
               P2OUT = LED2_BLUE;
              // make LED2 turning blue color
               break;
           case GREEN:
               P2OUT = LED2_GREEN;
              // make LED2 turning green color
               break;
           case PURPLE:
               P2OUT = LED2_RED | LED2_BLUE;
              // make LED2 turning purple color
               break;
           }
           color++; //increment the enum variable
           if(color == UNKNOW)
           {
              color = RED;
            } //if the enum variable finished a round, set the enum variable to red and get ready for next round.
      }


      // Keep the history of this button
      left_button_prev = left_button_cur;
      right_button_prev = right_button_cur;
  }
}
