#include <stdio.h>
#include <conio.h>
#include "ansi.h"

void main( void )
{
    int     c;


    printf("ANSI CODES TESTER\n\n");
    
    printf("Ready to start?...");  getch();
    ANSI_clrscr();
    
    ANSI_gotoxy(5,1);
    ANSI_setvideo(vidNormal);
    printf("This is NORMAL video");
    ANSI_gotoxy(5,2);
    ANSI_setvideo(vidHigh);
    printf("This is HIGH video");
    ANSI_gotoxy(5,3);
    ANSI_setvideo(vidUnderline);
    printf("This is UNDERLINE video");
    
    ANSI_gotoxy(5,4);
    ANSI_setvideo(vidNormal|vidBlink);
    printf("This is NORMAL BLINK video");
    ANSI_gotoxy(5,5);
    ANSI_setvideo(vidHigh|vidBlink);
    printf("This is HIGH BLINK video");

    ANSI_gotoxy(5,6);
    ANSI_setvideo(vidNormal|vidReverse);
    printf("This is NORMAL REVERSE video");
    

    ANSI_gotoxy(5,14);
    ANSI_setvideo(vidNormal);
    printf("Return to normal video");
}


