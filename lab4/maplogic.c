#include "maplogic.h"
#include <stdlib.h>
#include "sound.h"
#define DURATION 50

// This function returns true when a row contains three cells of state mark
int SameRow(tcellstate map[9], unsigned row, tcellstate mark) {
   unsigned start = (row - 1) * 3;
    return ((map[start  ] == map[start+1]) &&
            (map[start+1] == map[start+2]) &&
            (map[start  ] == mark));
}

// This function returns true when a column contains three cells of state mark
int SameCol(tcellstate map[9], unsigned col, tcellstate mark) {
    unsigned start = (col - 1);
    return ((map[start  ] == map[3+start]) &&
            (map[3+start] == map[6+start]) &&
            (map[start  ] == mark));
}

// This function returns true when a diagonal contains three cells of state mark
int SameDiag(tcellstate map[9], tcellstate mark) {
    return (((map[0] == map[4]) && (map[4] == map[8]) && (map[0] == mark)));
}

// This function returns true when a diagonal contains three cells of state mark
int SameDiagBack(tcellstate map[9], tcellstate mark) {
    return (((map[2] == map[4]) && (map[4] == map[6]) && (map[2] == mark)));
}

// This function returns true when X wins
int CrossWins(tcellstate map[9]) {
    if (SameRow(map, 1, cross))
        return 1;
    else if (SameRow(map, 2,   cross))
        return 2;
    else if (SameRow(map, 3,   cross))
        return 3;
    else if (SameCol(map, 1,   cross))
        return 4;
    else if (SameCol(map, 2,   cross))
        return 5;
    else if (SameCol(map, 3,   cross))
        return 6;
    else if (SameDiag(map,     cross))
        return 7;
    else if (SameDiagBack(map, cross))
        return 8;
    return 0;
}

// This function returns true when O wins
int CircleWins(tcellstate map[9]) {
    if (SameRow(map, 1, circle))
        return 1;
    else if (SameRow(map, 2,   circle))
        return 2;
    else if (SameRow(map, 3,   circle))
        return 3;
    else if (SameCol(map, 1,   circle))
        return 4;
    else if (SameCol(map, 2,   circle))
        return 5;
    else if (SameCol(map, 3,   circle))
        return 6;
    else if (SameDiag(map,     circle))
        return 7;
    else if (SameDiagBack(map, circle))
        return 8;
    return 0;
}

int Tie(tcellstate map[9]) {
    return ((map[0] != empty) &&
            (map[1] != empty) &&
            (map[2] != empty) &&
            (map[3] != empty) &&
            (map[4] != empty) &&
            (map[5] != empty) &&
            (map[6] != empty) &&
            (map[7] != empty) &&
            (map[8] != empty) &&
            (CircleWins(map) == 0)  &&
            (CrossWins (map) == 0));
}

// This function resets map sate to empty
void ClearMap(tcellstate map[9]) {
    map[0] = map[1] = map[2] = empty;
    map[3] = map[4] = map[5] = empty;
    map[6] = map[7] = map[8] = empty;
}

void AbortMap(tcellstate map[9]) {
    unsigned i;
    for (i=0; i<9; i++)
        if (map[i] == empty)
            map[i] = cross;
}

void sound(unsigned i)
{
    if(i==0) {
        PlaySound(note_c5,  DURATION);
        PlaySound(note_silent, 2*DURATION);
        PlaySound(note_g5,  DURATION);
    }
    else if(i == 1) {
        PlaySound(note_c5,  DURATION);
        PlaySound(note_silent, 2*DURATION);
        PlaySound(note_a5,  DURATION);
    }
    else if(i == 2) {
           PlaySound(note_c5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_b5, DURATION);
       }
    else if(i == 3) {
           PlaySound(note_d5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_g5,  DURATION);
       }
    else if(i == 4) {
           PlaySound(note_d5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_a5,  DURATION);
       }
    else if(i == 5) {
           PlaySound(note_d5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_b5,  DURATION);
       }
    else if(i == 6) {
           PlaySound(note_e5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_g5,  DURATION);
      }
    else if(i == 7) {
           PlaySound(note_e5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_a5,  DURATION);
       }
    else if(i == 8) {
           PlaySound(note_e5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_g5,  DURATION);
       }
    else if(i == 9) {
           PlaySound(note_e5,  DURATION);
           PlaySound(note_silent, 2*DURATION);
           PlaySound(note_a5,  DURATION);
       }
}

void ComputerMusic()
{
              PlaySound(note_g4,  2*DURATION);   // G4 // 2
              PlaySound(note_c5,  2*DURATION);   // C5 // 2
              PlaySound(note_e5,    DURATION);   // E5 // 1
              PlaySound(note_c5,    DURATION);   // C5 // 1
              PlaySound(note_g4,  2*DURATION);   // G4 // 2
              PlaySound(note_e5,  2*DURATION);   // E5 // 2
              PlaySound(note_c5,  2*DURATION);   // C5 // 2
              PlaySound(note_e5,    DURATION);   // E5 // 1
              PlaySound(note_c5,    DURATION);   // C5 // 1
              PlaySound(note_g4,  2*DURATION);   // G4 // 2
              PlaySound(note_e5,  2*DURATION);   // E5 // 2
              PlaySound(note_c5,  2*DURATION);   // C5 // 2
              PlaySound(note_e5,    DURATION);   // E5 // 1
              PlaySound(note_c5,    DURATION);   // C5 // 1
              PlaySound(note_g4,  2*DURATION);   // G4 // 2
              PlaySound(note_c5,  2*DURATION);   // C5 // 2
              PlaySound(note_e5,  4*DURATION);   // E5 // 4
              PlaySound(note_c5,  4*DURATION);   // C5 // 4
              PlaySound(note_g4,  3*DURATION);   // G4 // 3
              PlaySound(note_a4,    DURATION);   // A4 // 1
              PlaySound(note_b4,  2*DURATION);   // B4 // 2
              PlaySound(note_c5,  2*DURATION);   // C5 // 2
              PlaySound(note_d5,  2*DURATION);   // D5 // 2
              PlaySound(note_e5,  2*DURATION);   // E5 // 2
              PlaySound(note_f5,  2*DURATION);   // F5 // 2
              PlaySound(note_fs5, 2*DURATION);   // F#5 // 2
              PlaySound(note_g5, 16*DURATION);   // G5 // 16
}

void PlayerMusic()
{
    PlaySound(note_g4,  2*DURATION);   // G4 // 2
                PlaySound(note_a4,  2*DURATION);   // A4 // 2
                PlaySound(note_b4,  2*DURATION);   // B4 // 2
                PlaySound(note_c5,    DURATION);   // C5 // 1
                PlaySound(note_silent,DURATION);   // Silence // 1
                PlaySound(note_c5,  6*DURATION);   // C5 // 6
                PlaySound(note_e5,    DURATION);   // E5 // 1
                PlaySound(note_silent,DURATION);   // Silence // 1
                PlaySound(note_e5,  6*DURATION);   // E5 // 6
                PlaySound(note_f5,    DURATION);   // F5 // 1
                PlaySound(note_silent,DURATION);   // Silence // 1
                PlaySound(note_f5,  6*DURATION);   // F5 // 6
                PlaySound(note_fs5, 8*DURATION);   // F#5 // 8
                PlaySound(note_g5,  2*DURATION);   // G5 // 2
                PlaySound(note_g4,  2*DURATION);   // G4 // 2
                PlaySound(note_a4,  2*DURATION);   // A4 // 2
                PlaySound(note_c5,  2*DURATION);   // C5 // 2
                PlaySound(note_d5,  2*DURATION);   // D5 // 2
                PlaySound(note_c6, 10*DURATION);   // C6 // 10
                PlaySound(note_silent,2*DURATION); // Silence // 2
                PlaySound(note_c6,    DURATION);   // C6 // 1
}


// Adds symbol in a random empty location
int RandomAdd(tcellstate map[9], tcellstate v) {
   unsigned numempty = 0;
   unsigned done = 0;
   int value = -1;
   unsigned i;
   unsigned j;
   unsigned k;
   for (i=0; i<9; i++)
       if (map[i] == empty)
           numempty++;
   if (numempty == 0)
       return -1;
   while (!done) {
      // i = AI(map);
          for(i = 0, j = 1, k = 2; i<= 6, j<= 7, k<=8; i=i+3, j=j+3, k=k+3)
           {
               if (map[i] == map[j] && map[k] == empty && map[i] != empty) {
                   value = k;
                   break;
               }
               else if(map[i] == map[k] && map[j] == empty && map[i] != empty) {
                   value = j;
                   break;
               }
               else if(map[j] == map[k] && map[i] == empty && map[j] != empty){
                   value = i;
                   break;
               }
           }

           for(i = 0, j = 3, k = 6; i< 3, j< 6, k < 9; i++, j++, k++)
           {
               if (map[i] == map[j] && map[k] == empty && map[i] != empty)  {
                    value =  k;
                    break;
               }
               else if(map[i] == map[k] && map[j] == empty && map[i] != empty) {
                    value =  j;
                    break;
               }
               else if(map[j] == map[k] && map[i] == empty && map[j] != empty) {
                    value =  i;
                    break;
               }
           }
           if(value == -1)
           {
               value = rand() % 9;
               if(map[value] != empty)
               {
                   value = -1;
               }
           }
       if (map[value] == empty) {
           map[value] = v;
           done = 1;
       }
   }
   return value;

}

/*int AI(tcellstate map[9])
{   unsigned i;
    unsigned j;
    unsigned k;
    for(i = 1, j = 2, k = 3; i<= 7, j<= 8, k<=9; i=i+3)
    {
        if (map[i] == map[j] && map[k] == empty)
            value = k;
        else if(map[i] == map[k] && map[j] == empty)
            value = j;
        else if(map[j] == map[k] && map[i] == empty)
            value = i;
    }

    for(i = 1, j = 4, k = 7; i<= 3, j<= 6, k<=9; i++)
    {
        if (map[i] == map[j] && map[k] == empty)
             value =  k;
        else if(map[i] == map[k] && map[j] == empty)
             value =  j;
        else if(map[j] == map[k] && map[i] == empty)
             value =  i;
    }
    value =  rand() % 9;
}
*/












