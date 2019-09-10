#include <stdlib.h>
#include "swtimer.h"
#include "maplogic.h"
#include "button.h"
#include "display.h"
#include "hwtimer.h"
#include "goertzel.h"
#include "dtmf.h"
#include "microphone.h"
#include "sound.h"
// This is the state definition for the top-level state machine,
// implemented in ProcessStep. As you build the game, you will
// have to define additional states to implement the actual game
// logic.
typedef enum {idle} state_t;
unsigned glbListening = 0;
int finished = 0;
int choose = -1;
// This structure is a game state. It keeps track of the current
// playing field, the score achieved by the human, and the score
// achieved by the computer. The game state gets passed around
// during the game to allow different software components access
// to the game state.

typedef struct {
    tcellstate map[9];
    int computerscore;
    int humanscore;
} gamestate_t;


// This function implements the functionality of Tic Tac Tone during
// the idle state, i.e. when it is waiting for the player to provide
// a 'start game' command.
//
// This function is called every time the top-level FSM visits the
// idle state. The function has five parameters. You are welcome to
// add more, but know that the reference solution was implemented using
// only these 5 parameters.
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses
//    G    = gamestate, as defined above. Pass by reference to make
//           sure that changes to G within this function will be
//           propagated out of the function.
//
// Note that this function RETURNS a state_t. This can be used to influence
// the state transition in the top-level FSM that will call processIdle.
// Currently, the ProcessIdle always returns idle (which means the top-level
// FSM will iterate in the idle state), but when the game is extended,
// the return state could change into something different (such as playing_circle
// or playing_cross, depending on whether S1 or S2 is pressed).




state_t ProcessIdle(int b1, int b2,
                    int sec, int ms50,
                    gamestate_t *G) {

    // These are the states of a _local_ FSM.
    // The state labels are prefixed with 'processidle' to
    // make sure they are distinct from the labels used for the
    // top-level FSM.
    //
    // The local FSM functionality plays a game of tic-tac-toe
    // against itself, using randomized moves. However, the
    // rules of tic-tac-toe are followed, including the game
    // map drawing and coloring over the reference solution.

    typedef enum {processidle_idle,
                  processidle_playingcircle,
                  processidle_playingcross,
                  processidle_winning} processidle_state_t;
    static processidle_state_t localstate = processidle_idle;

    unsigned w;

    // We will run this local state machine only once per second,
    // therefore, we only run it when sec is 1. sec is
    // a software-timer generated in the main function.
    //
    // To add more functionality, you can extend this function. For example,
    // to display a label every three seconds, you can add a counter that is
    // incremented for every sec, modulo-3. When the counter is two, it means
    // that the three-second timer mark is reached.
    //
    // A longer counter period (eg modulo-12 iso modulo-3) can be used to
    // display rotating messages.

    if (sec) {

        switch (localstate) {

            case processidle_idle:
              // Initially, just draw the playing field

              ClearMap(G->map);
              DrawBoard(G->map);
              localstate = processidle_playingcircle;
              break;

            case processidle_playingcircle:
              // This is circle who is playing. A circle is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, circle);

              // Next, it's cross' turn
              localstate = processidle_playingcross;

              // If we win or tie, go to winning state instead
              if (CircleWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_playingcross:
              // This is cross who is playing. A cross is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, cross);

              // Next, it's circles' turn
              localstate = processidle_playingcircle;

              // If we win or tie, go to winning state instead
              if (CrossWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_winning:
              // This state is entered when there is a winner,
              // or it's a tie. In this state, we redraw the
              // winning combination in the emphasis color.
              // After that, we go for the next round.

              if (w = CircleWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              if (w = CrossWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              localstate = processidle_idle;
              break;

        }
    }

    return idle;

}

int Ifilligal(tcellstate map[9], int i)
{
    if(map[i-1] == empty)
    {
        return  1;
    }
    else
    {
        return 0;
    }
}

void abortGame(tcellstate map[9])
{
    int i = 0;
    for(i; i < 9; i++)
    {
        if(map[i] == empty)
        {
          map[i] = cross;
        }
    }
}


void processComputer(int b1, int b2, int sec, int ms50, gamestate_t *G)
{
    tSWTimer everysecond;
    InitSWTimer(&everysecond, TIMER32_1_BASE, SYSTEMCLOCK);
    typedef enum {processidle_idle, processidle_playingcircle, processidle_playingcross,
                      processidle_winning, processAbort} processidle_state_t;

    static processidle_state_t localstate = processidle_idle;
    unsigned w;
    int intt = 0;
    int illigal;
    int temp;
    int note;
       if (sec) {
           switch (localstate) {
               case processidle_idle:
                 ClearMap(G->map);
                 DrawBoard(G->map);
                 if(choose == 0)
                 localstate = processidle_playingcross;
                 else if(choose == 1){
                     localstate = processidle_playingcircle;
                 }
                 break;

               case processidle_playingcircle:
                    DrawMessage(" Listening.... ", EMPHASISCOLOR);
                    //intt = check();
                       if (!glbListening){
                        intt =  DTMFDisplay();
                        DTMFReset();
                        glbListening = 1;
                       }
                  if (intt >= 1 && intt <= 9){
                        illigal = Ifilligal(G->map, intt);
                        if(illigal == 1 ){
                            temp = intt;
                            G->map[intt-1] = circle;
                            DrawBoard(G->map);
                            intt = 0;
                            localstate = processidle_playingcross;
                        }
                       else if(illigal == 0 && intt != temp){
                            StartSWTimer(&everysecond);
                            while(!SWTimerExpired(&everysecond)){
                                DrawMessage(" illegal.... ", EMPHASISCOLOR);
                            }
                       }
                    }
                 if (CircleWins(G->map) || Tie(G->map))
                     localstate = processidle_winning;
                   //  DrawBoard(G->map);
                 if(ButtonS1Pressed())
                     localstate = processAbort;
                 break;
               case processidle_playingcross:
                 StartSWTimer(&everysecond);
                 while(!SWTimerExpired(&everysecond)){
                     DrawMessage(" Thinking.... ", EMPHASISCOLOR);
                 }
                note = RandomAdd(G->map, cross);
                sound(note);
                localstate = processidle_playingcircle;
                 if (CrossWins(G->map) || Tie(G->map))
                     localstate = processidle_winning;
                 DrawBoard(G->map);
                 if(ButtonS1Pressed())
                    localstate = processAbort;
                 break;
               case processidle_winning:
                 if (w = CircleWins(G->map)) {
                   G->humanscore++;
                   DrawMessage("   You win!   ", EMPHASISCOLOR);
                   DrawWinner(G->map, w, EMPHASISCOLOR);
                   PlayerMusic();
                 }
                 else if (w = CrossWins(G->map)) {
                   G->computerscore++;
                   DrawMessage("   You lose!    ", EMPHASISCOLOR);
                   DrawWinner(G->map, w, EMPHASISCOLOR);
                   ComputerMusic();
                 }
                 else {
                     DrawMessage("   Tie Game!    ", EMPHASISCOLOR);
                 }
                 finished = 1;
                 localstate = processidle_idle;
                 break;
               case processAbort:
                   abortGame(G->map);
                   DrawBoard(G->map);
                   if (CircleWins(G->map) || Tie(G->map))
                        localstate = processidle_winning;
                   if (CrossWins(G->map) || Tie(G->map))
                        localstate = processidle_winning;
           }
      }
}




void ProcessStep(int b1, int b2, int sec, int ms50, int sec3) {
    typedef enum {idle, computerStarts, playerStarts} process_state_t;
    static process_state_t S = idle;
    static gamestate_t G;

    switch (S) {

    case idle:
        S = ProcessIdle(b1, b2, sec, ms50, &G);
        drawMessage(sec3, &G);
        if(b1) {
           choose = 0;
           S = computerStarts;
        }
        if(b2) {
           choose = 1;
           S = computerStarts;
        }
        break;
    case computerStarts:
       processComputer(b1, b2, sec, ms50, &G);
       if(finished == 1){
       S = idle;
       finished = 0;
       }
       break;
 //   case playerStarts:
    }
}

void T32_INT1_IRQHandler() {
    unsigned vx;
    static unsigned SamplesListened = 0;
    if (glbListening) {
        vx = GetSampleMicrophone();
        DTMFAddSample(vx);
        SamplesListened++;
        if (SamplesListened == 400) {
            glbListening = 0;
            SamplesListened = 0;
        }
    }
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}

int check()
{
   // DTMFReset();
    int returnvalue = -1;
    if (!glbListening) {
        returnvalue =  DTMFDisplay();
        DTMFReset();
        glbListening = 1;
        return returnvalue;
        }
    return returnvalue;
}

void drawMessage(int sec3, gamestate_t *G)
{
    typedef enum {TicTacToe, Score, I,You} message_state_t;
    static message_state_t message = TicTacToe;
    char *computer = G->computerscore + '0';
    //char human = G->humanscore + '0';
    //char *score = "Score" + computer + human;
    switch(message) {
    case TicTacToe:
        DrawMessage("  *TicTacToe*  ", EMPHASISCOLOR);
        if(sec3)
            message =  Score;
        break;
    case Score:
        DrawScore(G->computerscore, G->humanscore, EMPHASISCOLOR);
       // DrawMessage( "Score 00-00", EMPHASISCOLOR);
        if(sec3)
            message =  I;
        break;
    case I:
         DrawMessage(" S1: I Start   ", EMPHASISCOLOR);
         if(sec3)
            message =  You;
         break;
    case You:
         DrawMessage(" S2: You Start ", EMPHASISCOLOR);
         if(sec3)
            message = TicTacToe;
         break;
    }
}

int main(void) {
    InitTimer();
    InitDisplay();
    InitButtonS1();
    InitButtonS2();
    InitMicrophone();
    InitSound();

    Interrupt_enableInterrupt(INT_T32_INT1);
    Interrupt_enableMaster();
    glbListening = 1;

    tSWTimer everySec;
    InitSWTimer(&everySec, TIMER32_1_BASE, SYSTEMCLOCK);
    StartSWTimer(&everySec);

    // Software Timer - per 50ms = 20Hz
    tSWTimer every50ms;
    InitSWTimer(&every50ms, TIMER32_1_BASE, SYSTEMCLOCK/20);
    StartSWTimer(&every50ms);

    tSWTimer every3seconds;
    InitSWTimer(&every3seconds, TIMER32_1_BASE, SYSTEMCLOCK*3);
    StartSWTimer(&every3seconds);


    // The cyclic executive is simple: read the buttons and software
    // timers, and call the top-level FSM in ProcessStep.
    while (1) {
      //  check();
        int b1    = ButtonS1Pressed();
        int b2    = ButtonS2Pressed();
        int sec   = SWTimerExpired(&everySec);
        int ms50  = SWTimerExpired(&every50ms);
        int sec3  = SWTimerExpired(&every3seconds);

        ProcessStep(b1, b2, sec, ms50, sec3);
    }
}
