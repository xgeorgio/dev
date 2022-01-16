/*
    FILE:           'hffreq.c'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Character frequency analyzer
    VERSION:        1.0
    UPDATED:        23-July-2003/14:55

    DESCRIPTION:    Main source file for file I/O.

    AUTHOR:         Harris Georgiou (c) 2003
*/

#include "hffreq.h"

//... Standard usage display ...
void usage( void )
{
    printf("USAGE:\n\thffreq <filename>\n\n");
}


//...  Main program ...
int main(int argc, char* argv[])
{
    int            ch, errors=0;
    unsigned long  chars[UCHAR_MAX+1];


    printf("%s - %s\nversion %s %d.%d.%d.%d / %s / %s\n%s\n\n",
        APP_VERSION_NAME,APP_TAGINFO,APP_VERSION_STR,APP_VERSION_MAJOR,APP_VERSION_MINOR,
        APP_VERSION_BUILD,APP_VERSION_RELEASE,CPUID_ISET_STR,APP_VERSION_DATE,APP_COPYRIGHT);

    // initialize counters, just in case the compiler does not do so by default
    for ( ch=0; ch<UCHAR_MAX+1; ch++ )  chars[ch]=0;

    // main counting loop
    ch=fgetc(stdin);
    while(ch!=EOF)
    {
        if (ch>UCHAR_MAX)
            errors++;    // bad implementation of fgetc(), negative return not EOF
        else
            chars[ch]++;
        ch=fgetc(stdin);
    }

    // display only non-zero counters
    for ( ch=0; ch<UCHAR_MAX+1; ch++ )
    {
        if (chars[ch])  printf("%02X %c %ld\n",ch,((ch>=0x20)?ch:'?'),chars[ch]);
    }

    return (0);
}
