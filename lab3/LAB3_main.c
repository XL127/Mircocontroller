/*
 * You can add more of your own developed *.h files here.
 * However, you are not allowed to include driverlib, grlib, stdio or any other library in this file.
 */

#include <LED_HAL.h>
#include <Buttons_HAL.h>
#include <Timer_HAL.h>
#include <Display_HAL.h>

extern Graphics_Image tree4BPP_UNCOMP;
extern Graphics_Image car4BPP_UNCOMP;


#define OPENING_WAIT 3000 // 10 seconds or 10000 ms
#define MESSAGE_WAIT 1000
#define TreeTimer 50
#define CarTimer  50
#define UP_THRESHOLD  0x3000
#define DOWN_THRESHOLD 0x1000
#define LEFT_THRESHOLD 0x1000
#define RIGHT_THRESHOLD 0x3000

unsigned vx, vy;
int selectRow;
int expired = 0;
int check =0;
int select = 2;
int y1 = 0;
int y2 = 0;
int y3 = 0;
int x1 = 0;
int x2 = 0;
int x3 = 0;
int x4 = 0;
int y4 = 0;
int CarX = 18;
int TotalCount = 0;
int first = 0;
int lives = 3;
bool lose  = false;
bool print = true;
bool One = false;
bool Two = false;
bool Three = false;
bool pass = false;
int ScoreArray[4] = {0, 0, 0, 0};

Graphics_Context g_sContext;


bool ButtonPressed() {
    return (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN1)==0);
} //check if Button1 is pressed


void clearScreen()
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    PrintString(">", 2, 1);
    PrintString(">", 4, 1);
    PrintString(">", 6, 1);
} //clear the screen with the Foreground color of black

void DrawOpeningScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("Car Game", 2, 2);
    PrintString("by", 3, 5);
    PrintString("Xinwei Lu", 4, 2);
    PrintString("Wait 3 sec", 6, 1);
} //Draw the opening screen

void DrawMenuScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("GAME MENU", 0, 2);
    PrintString(">", selectRow, 1);
    PrintString("Play Game", 2, 3);
    PrintString("How to Play", 4, 3);
    PrintString("Score History",6, 3);
} //draw the menu screen

void ResetGame()
{
    TotalCount = 0;
    y1 = 0;
    y2 = 0;
    y3 = 0;
    One = false;
    Two = false;
    Three = false;
    lose = false;
    ScoreArray[0] = 0;
    lives = 3;
} //reset the all the game varible

int Random()
{
    getSampleJoyStick(&vx, &vy);
    int Binary[2] = {};
    Binary[0] =  (vx%2) ^ (vy%2);
    Binary[1] = !((vx%2) ^ (vy&2));
   // Binary[2] = (!vx%2) ^ (vx%2);
   int Total = Binary[0] * 1 + Binary[1] * 2;
    if(Total == 0)
        return 0;
    else if(Total == 1)
        return 16;
    else if (Total == 2)
        return 32;
    else
        return 16;
} //produce a random number

void MenuSelection()
{
    if(vy < UP_THRESHOLD && vy > DOWN_THRESHOLD)
          check = 1;

    getSampleJoyStick(&vx, &vy);

    if(check == 1 && expired == 1) {
        selectOption();
         check = 0;
                   }
}

void selectOption()
{
   // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
   // PrintString(">", selectRow, 1);
    if( vy > UP_THRESHOLD)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", selectRow, 1);
        if(selectRow == 2){
            selectRow = 6;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            PrintString(">", 6, 1);
        }
        else if(selectRow == 4){
            selectRow = 2;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            PrintString(">", 2, 1);
        }
        else if(selectRow == 6){
            selectRow = 4;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            PrintString(">", 4, 1);
        }
    }
    else if (vy < DOWN_THRESHOLD)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", selectRow, 1);
        //Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        if(selectRow == 2){
           selectRow = 4;
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
           PrintString(">", 4, 1);
           }
         else if(selectRow == 4){
            selectRow = 6;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            PrintString(">", 6, 1);
           }
         else if(selectRow == 6){
            selectRow = 2;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
            PrintString(">", 2, 1);
          }
    }
} //The menu state mechine selection, move the joystick down and up to select the options.


//The folllowing is the main FSM
//There are eight states in this state machine, each state represents a screen panel
void ScreensFSM(){
       static enum states {INCEPTION, OPENING, MENU, How, Score, Play, GameOver, Pause} state = INCEPTION;
       static OneShotSWTimer_t OST;
       InitOneShotSWTimer(&OST, TIMER32_1_BASE, OPENING_WAIT);
       bool drawOpening = false;
       bool drawMenu = false;

    switch (state)  {
    case INCEPTION:
        StartOneShotSWTimer(&OST);
        state = OPENING;// State transition
        drawOpening = true;// The output(s) that are affected in this transition
        break;
    case OPENING:
        if (OneShotSWTimerExpired(&OST)){
            state = MENU; // State transition
            expired = 1;  // The output(s) that are affected in this transition
            drawMenu = true;
        }
        break;
    case MENU:
               MenuSelection(); //Select the options on the menu
               if (ButtonPressed()){
                   if(selectRow == 4)
                       state = How;
                   else if(selectRow == 6)
                       state = Score;
                   else if(selectRow == 2){
                       state = Play;
                   } //check if the corrsponding options is select
                }
        break;
    case How:
        HowToPlay();
        if(Booster_Top_Button_Pushed()){
            print = true;
            drawMenu = true;
            state = MENU;
        }
        break; //the HowToPlay state
    case Score:
        ScoreHistory();
        if(Booster_Top_Button_Pushed()){
            print = true;
            drawMenu = true;
            state = MENU;
        }
        break; //jump to the scoreHistory panel
    case Play:
        PlayGame();
        if(lose == true)
        state = GameOver;
        else {
        state = Pause;
        }
        break; //jump to the play panel
    case GameOver:
        DrawOverMessage();
        DrawGameOver();
        drawMenu = true;
        compareScore();
        ResetGame();
        state = MENU;
        break;
    case Pause:
        DrawPause();
        select = PauseSelect(select);
        if(ButtonPressed()){
             if(select == 2){
                state = Play;
                print = true;
                select = 2;
             }
             else if(select == 4) {
                state = MENU;
                drawMenu = true;
                print = true;
                select = 2;
                ResetGame();
             }
         }
       break;
    }   // End of switch-case
    if (drawOpening)
       DrawOpeningScreen();
    if (drawMenu)
        DrawMenuScreen();
 }

//The following is Pause section

int PauseSelect(int select)
{
    if(vy < UP_THRESHOLD && vy > DOWN_THRESHOLD)
    {
        check = 1;
    }

     getSampleJoyStick(&vx, &vy);

     if(check == 1 && expired == 1)
     {
         if( vy > UP_THRESHOLD)
            {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                PrintString(">", select, 1);
                if(select == 2){
                    select = 4;
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                    PrintString(">", 4, 1);
                }
                else if(select == 4){
                    select = 2;
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                    PrintString(">", 2, 1);
                }
            }
            else if (vy < DOWN_THRESHOLD)
            {
                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                PrintString(">", select,1);
                if(select == 2){
                   select = 4;
                   Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                   PrintString(">", 4, 1);
                   }
                 else if(select == 4){
                    select = 2;
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                    PrintString(">", 2, 1);
                   }
            }
         check = 0;
         return select;
     }
     return select;
}
//pause the game

int DrawPause()
{
    int returnValue = 0;
    static int select = 2;
    if(print == true)
    {
    LCDClearDisplay(MY_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    PrintString(">", 2,1);
    PrintString("GAME PAUSED", 0 ,2);
    PrintString("Continue", 2, 2);
    PrintString("Menu", 4, 2);
    print = false;
    }
}



//The following is GameOver =section

void DrawGameOver()
{
    static OneShotSWTimer_t OST;
    InitOneShotSWTimer(&OST,TIMER32_1_BASE,OPENING_WAIT);

    LCDClearDisplay(MY_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    PrintString("GAME OVER", 0 ,2);
    PrintString ("Score: ", 2, 2);
    char digit1 = ScoreArray[0]/1 % 10 + '0'; //unit
    char digit2 = ScoreArray[0]/ 10 % 10 + '0'; //ten
    LCDDrawChar(2, 10, digit1);
    LCDDrawChar(2, 9, digit2);
   // LCDDrawChar(3,5, ScoreArray[0]+'0');
    StartOneShotSWTimer(&OST);
    while(1)
    {
        if (Booster_Bottom_Button_Pushed())
                    Toggle_Launchpad_Right_Red_LED();
        if(OneShotSWTimerExpired(&OST))
        {
            break;
        }
    }
}

void DrawOverMessage()
{
        static OneShotSWTimer_t OST;
        InitOneShotSWTimer(&OST,TIMER32_1_BASE,MESSAGE_WAIT);

        LCDClearDisplay(MY_BLACK);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        //PrintString("Game Over", 1 ,1);
        PrintString ("B O O M ! Hit!", 3, 2);
       // LCDDrawChar(3,5, ScoreArray[0]+'0');
        StartOneShotSWTimer(&OST);
        while(1)
        {
            if (Booster_Bottom_Button_Pushed())
                        Toggle_Launchpad_Right_Red_LED();
            if(OneShotSWTimerExpired(&OST))
            {
                break;
            }
        }
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
}
//draw the message
//The following is HowToPlay section
void HowToPlay()
{
   // static bool print = true;
   // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
   // PrintString(">", 4, 1);
    if(print == true)
    {
    LCDClearDisplay(MY_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    PrintString("INSTRUCTIONS", 0 ,2);
    PrintString("Move the Car to", 1, 0);
    PrintString("   avoid a crush", 2, 0);
    PrintString("You have", 3, 0);
    PrintString("three lives", 4, 0);
    PrintString("BTN1-MENU", 7, 3);
    print = false;
    }
}
//print how to play section
//The following is Socre section
void ScoreHistory()
{
    char digit1 = ScoreArray[1]/1 % 10 + '0'; //unit
    char digit2 = ScoreArray[1]/ 10 % 10 + '0'; //ten
    char digit3 = ScoreArray[2]/1 % 10 + '0'; //unit
    char digit4 = ScoreArray[2]/ 10 % 10 + '0'; //ten
    char digit5 = ScoreArray[3]/1 % 10 + '0'; //unit
    char digit6 = ScoreArray[3]/ 10 % 10 + '0'; //ten
   // static bool print = true;
    if(print == true) {
    LCDClearDisplay(MY_BLACK);
    PrintString("Score History", 1 ,1);
    PrintString("First: ", 2,1);
    PrintString("Second: ",4,1);
    PrintString("Third: ", 6,1);
    PrintString("BTN1-MENU", 7, 3);
    LCDDrawChar(6,8, digit1);
    LCDDrawChar(6,7, digit2);
    LCDDrawChar(4,9, digit3);
    LCDDrawChar(4,8, digit4);
    LCDDrawChar(2,8, digit5);
    LCDDrawChar(2,7, digit6);
    //LCDDrawChar(5,5,ScoreArray[0] + '0');
    print = false;
    }
}
//print the history panel

void compareScore()
{
    int min = ScoreArray[1];
    int medium = ScoreArray[2];
    int max = ScoreArray[3];
    if(ScoreArray[0] > max)
    {
        min = medium;
        medium = max;
        max = ScoreArray[0];
    }
    else if(ScoreArray[0] > medium)
    {
        min = medium;
        medium = ScoreArray[0] ;
    }
    else if(ScoreArray[0] > min)
    {
        min =  ScoreArray[0];
    }
    ScoreArray[1] = min;
    ScoreArray[2] = medium;
    ScoreArray[3] = max;
}

//The folllowing is Game section

void PlayGame()
{
        LCDClearDisplay(MY_BLACK);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        DrawGame();
        StartGame();
}
//start the game
void DrawGame()
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_Rectangle rectan;
    rectan.yMax = 127;
    rectan.yMin = 0;
    rectan.xMin = 48;
    rectan.xMax = 49;
    Graphics_drawRectangle(&g_sContext,&rectan);
    PrintString("SCORE", 0, 8);
    PrintString("HIGH",  3, 8);
    PrintString("SCORE", 4, 8);
    PrintString("Lives", 6, 8);
    LCDDrawChar(1,8, '0');
    LCDDrawChar(1,7, '0');
    char digit1 = ScoreArray[3]/1 % 10 + '0'; //unit
    char digit2 = ScoreArray[3]/ 10 % 10 + '0'; //ten
    LCDDrawChar(5, 8, digit1);
    LCDDrawChar(5, 7, digit2);
   // LCDDrawChar(5,8,ScoreArray[3]+'0');
    LCDDrawChar(7,8,lives+'0');
}
//draw the game panel

void StartGame(){
    static OneShotSWTimer_t OST;
    InitOneShotSWTimer(&OST, TIMER32_1_BASE, TreeTimer);

    StartOneShotSWTimer(&OST);

    while (1) {

        if (Booster_Bottom_Button_Pushed())
            Toggle_Launchpad_Right_Red_LED();

        if(Booster_Top_Button_Pushed())
            break;

        if(OneShotSWTimerExpired(&OST)){
            if(TotalCount == 0) {
                One = true;
                       }
            if(TotalCount == 50 ){
                Two = true;
                       }
            if(TotalCount == 100){
                Three = true;
                       }
            if(One == true)
            {
                moveTreeOne();
            }
            if (Two == true)
            {
                moveTreeTwo();
            }
            if (Three == true)
            {
                moveTreeThree();
            }
          StartOneShotSWTimer(&OST);
          TotalCount++;
          moveCar();
        }
        if(pass == true  && (y1 == 127 || y2 == 127 || y3 == 127))
        {
          pass = false;
          lives--;
          Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
          LCDDrawChar(7,8, lives+'0');
        }
        if(CheckIfhit() == 1)
        {
            lose = true;
            break;
        }
    }
}
 //start the game and check if the car hits a tree


int moveCar()
{
    Graphics_drawImage(&g_sContext, &car4BPP_UNCOMP, CarX, 111);
    //static OneShotSWTimer_t OST;
    //InitOneShotSWTimer(&OST, TIMER32_1_BASE, CarTimer);
    //StartOneShotSWTimer(&OST);
    getSampleJoyStick(&vx, &vy);

    if(vx < LEFT_THRESHOLD)
    {

        Graphics_Rectangle Rectan;
        Rectan.yMax = 127;
        Rectan.yMin = 111;
        Rectan.xMin = CarX+16;
        Rectan.xMax = CarX+31;
        if(CarX+32 > 48){
            Rectan.xMax = 46;
            Rectan.xMin = CarX+15;
        }

        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawRectangle(&g_sContext,&Rectan);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);

        if(CarX == 0)
         CarX = 0;
        else {
        CarX--;
        }
    }
    else if(vx > RIGHT_THRESHOLD)
    {
        Graphics_Rectangle Rectan;
        Rectan.yMax = 127;
        Rectan.yMin = 111;
        Rectan.xMax = CarX;
        Rectan.xMin = CarX-16;

        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawRectangle(&g_sContext,&Rectan);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        if(CarX == 32)
            CarX = 32;
        else {
        CarX++;
        }
    }
} //move the car

int moveTreeOne(){
     //  unsigned i;
      // static unsigned x = 10;//, //y = 0;
    //   y = inputY;
       Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

       Graphics_Rectangle Rectan;
       Rectan.yMax = y1;
       Rectan.yMin = y1 - 16;
       Rectan.xMin = x1;
       Rectan.xMax = x1+16;

       Graphics_drawRectangle(&g_sContext,&Rectan);

       y1++;
       if(y1 > 150) {
          y1 = 0;
          x1 = Random();
          //Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
          //LCDDrawChar(2, 10, y1);
          //Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        }
       if(y1 == 127)
       {
          ScoreArray[0] = ScoreArray[0] + 1;
          Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
          char digit1 = ScoreArray[0]/1 % 10 + '0'; //unit
          char digit2 = ScoreArray[0]/ 10 % 10 + '0'; //ten
          LCDDrawChar(1, 8, digit1);
          LCDDrawChar(1, 7, digit2);
          Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
       }
       if(ScoreArray[0] > ScoreArray[3])
       {
          Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
          char digit3 = ScoreArray[0]/1 % 10 + '0'; //unit
          char digit4 = ScoreArray[0]/ 10 % 10 + '0'; //ten
          LCDDrawChar(5, 8, digit3);
          LCDDrawChar(5, 7, digit4);
          Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
       }

       Graphics_drawImage(&g_sContext, &tree4BPP_UNCOMP, x1, y1);
       return y1;
} //move the first tree

int moveTreeTwo()
{
       // static unsigned x = 15;//, //y1 = 0;
      //  y = inputY;
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

        Graphics_Rectangle Rectan;
        Rectan.yMax = y2;
        Rectan.yMin = y2 - 16;
        Rectan.xMin = x2;
        Rectan.xMax = x2+16;

        Graphics_drawRectangle(&g_sContext,&Rectan);

        y2++;
        if(y2 > 150) {
           y2 = 0;
           x2 = Random();
          // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
          // LCDDrawChar(2, 10, y1);
          // Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
         }

        if(y2 == 127)
         {
           ScoreArray[0] = ScoreArray[0] + 1;
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
           char digit1 = ScoreArray[0]/1 % 10 + '0'; //unit
           char digit2 = ScoreArray[0]/ 10 % 10 + '0'; //ten
           LCDDrawChar(1, 8, digit1);
           LCDDrawChar(1, 7, digit2);
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
         }
         if(ScoreArray[0] > ScoreArray[3])
         {
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
           char digit3 = ScoreArray[0]/1 % 10 + '0'; //unit
           char digit4 = ScoreArray[0]/ 10 % 10 + '0'; //ten
           LCDDrawChar(5, 8, digit3);
           LCDDrawChar(5, 7, digit4);
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
         }

        Graphics_drawImage(&g_sContext, &tree4BPP_UNCOMP, x2, y2);
        return y2;
} //move the second tree

int moveTreeThree()
{
           // static unsigned x = 8;//, y = 0;
          //  y = inputY;
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

            Graphics_Rectangle Rectan;
            Rectan.yMax = y3;
            Rectan.yMin = y3 - 16;
            Rectan.xMin = x3;
            Rectan.xMax = x3+16;

            Graphics_drawRectangle(&g_sContext,&Rectan);

            y3++;
            if(y3 > 150) {
               y3 = 0;
               x3 = Random();
             }

             if(y3 == 127)
             {
                 ScoreArray[0] = ScoreArray[0] + 1;
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                 //char digit = ScoreArray[0] + '0';
                 char digit1 = ScoreArray[0]/1 % 10 + '0'; //unit
                 char digit2 = ScoreArray[0]/ 10 % 10 + '0'; //ten
                 LCDDrawChar(1, 8, digit1);
                 LCDDrawChar(1, 7, digit2);
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
             }
             if(ScoreArray[0] > ScoreArray[3])
             {
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                 char digit3 = ScoreArray[0]/1 % 10 + '0'; //unit
                 char digit4 = ScoreArray[0]/ 10 % 10 + '0'; //ten
                 LCDDrawChar(5, 8, digit3);
                 LCDDrawChar(5, 7, digit4);
                 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
             }
            Graphics_drawImage(&g_sContext, &tree4BPP_UNCOMP, x3, y3);
            return y3;
}
//move the third tree

int CheckIfhit()
{
    if(y1 < 127 && y1 >= 95 && abs(CarX - x1) < 16)
    {
        if(lives > 1)
        {
            pass = true;
            return 2;
        }
       else if ( lives == 1)
       {
        return 1;
       }
    }
    else if(y2 < 127 && y2 >= 95 && abs(CarX - x2) < 16)
    {
        if(lives > 1)
        {
            pass = true;
            return 2;
         }
        else if (lives == 1)
        {
          return 1;
        }
    }
    else if(y3 < 127 && y3 >= 95 && abs(CarX - x3) < 16)
    {
        if(lives > 1)
         {
           pass  = true;
           return 2;
         }
        else if(lives == 1)
        {
           return 1;
        }
    }
    else
    {
        return 0;
    }
    return 0;
}
 //check if the car hits the tree

//The main function

int main(void) {

    WDT_A_hold(WDT_A_BASE);

    BSP_Clock_InitFastest();
    InitGraphics();
    InitHWTimers();
    InitButtons();
    InitLEDs();
    initADC_Multi();
    initJoyStick();
    startADC();
    selectRow = 2;
    //int check = 0;

    x1 = Random();
    x2 = Random();
    x3 = Random();
    x4 = Random();
    while (1) {
        // Do not delete this statement. We will use this function to check if your program does not block on anything.
        if (Booster_Bottom_Button_Pushed())
            Toggle_Launchpad_Right_Red_LED();
       // getSampleJoyStick(&vx, &vy);
        ScreensFSM();
       /* if(Booster_Top_Button_Pushed())
         DrawMenuScreen();*/
    }
}
