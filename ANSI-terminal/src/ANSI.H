#ifndef     _ANSI_H
#define     _ANSI_H

#include <stdio.h>


#define         ANSI_GOTOXY         "\033[%02d;%02dH"       /* or: "\033[%02d;%02dF" */
#define         ANSI_CUR_UP         "\033[%02dA"
#define         ANSI_CUR_DN         "\033[%02dB"
#define         ANSI_CUR_RT         "\033[%02dC"
#define         ANSI_CUR_LT         "\033[%02dD"
#define         ANSI_CUR_SPOS       "\033[s"
#define         ANSI_CUR_RPOS       "\033[u"
#define         ANSI_CLRSCR         "\033[2J"
#define         ANSI_CLRLIN         "\033[K"

#define         ANSI_CHARNORM       "\033[0m"
#define         ANSI_CHARBOLD       "\033[1m"
#define         ANSI_CHARBLNK       "\033[5m"
#define         ANSI_CHARINVR       "\033[7m"
#define         ANSI_CHARULIN       "\033[34m"

#define         ANSI_gotoxy(x,y)            printf(ANSI_GOTOXY,y+1,x+1)
#define         ANSI_cursor_up()            printf(ANSI_CUR_UP)
#define         ANSI_cursor_down()          printf(ANSI_CUR_DN)
#define         ANSI_cursor_right()         printf(ANSI_CUR_RT)
#define         ANSI_cursor_left()          printf(ANSI_CUR_LT)
#define         ANSI_cursor_save()          printf(ANSI_CUR_SPOS)
#define         ANSI_cursor_restore()       printf(ANSI_CUR_RPOS)
#define         ANSI_clrscr()               printf(ANSI_CLRSCR)
#define         ANSI_delline()              printf(ANSI_CLRLIN)

#define         ANSI_video_normal()         printf(ANSI_CHARNORM)
#define         ANSI_video_high()           printf(ANSI_CHARBOLD)
#define         ANSI_video_blink()          printf(ANSI_CHARBLNK)
#define         ANSI_video_reverse()        printf(ANSI_CHARINVR)
#define         ANSI_video_underline()      printf(ANSI_CHARULIN)


#define         vidNormal       0
#define         vidHigh         1
#define         vidBlink        2
#define         vidReverse      4
#define         vidUnderline    8

void ANSI_setvideo( int mode )
{
    ANSI_video_normal();
    if ( mode & vidHigh )  ANSI_video_high();
    if ( mode & vidBlink )  ANSI_video_blink();
    if ( mode & vidReverse )  ANSI_video_reverse();
    if ( mode & vidUnderline )  ANSI_video_underline();
}



#endif




