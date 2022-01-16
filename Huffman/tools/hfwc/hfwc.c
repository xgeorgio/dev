/*
    FILE:           'hfwc.c'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Lines/words/characters counter program
    VERSION:        1.1
    UPDATED:        23-July-2003/14:55

    DESCRIPTION:    Main source file for file I/O and lines/words/chars count.

    AUTHOR:         Harris Georgiou (c) 2003
*/

#include "hfwc.h"

//... Standard usage display ...
void usage( void )
{
    printf("USAGE:\n\thfwc <filename>\n\n");
}


//...  Main program ...
int main(int argc, char* argv[])
{
    int            ch, inword=0;
    unsigned long  lines=0, words=0, chars=0;


    printf("%s - %s\nversion %s %d.%d.%d.%d / %s / %s\n%s\n\n",
        APP_VERSION_NAME,APP_TAGINFO,APP_VERSION_STR,APP_VERSION_MAJOR,APP_VERSION_MINOR,
        APP_VERSION_BUILD,APP_VERSION_RELEASE,CPUID_ISET_STR,APP_VERSION_DATE,APP_COPYRIGHT);

    ch=fgetc(stdin);
    while(ch!=EOF)
    {
        if (lines==0)  lines++;
        switch(ch)
        {
            case  EOF: if (chars>0)  words++;     // close any final word without line change
                       break;
            case '\r':
            case '\n': lines++;  inword=0;        // close word, increase line count
            case '\t':
            case  ' ': if (chars>0) { words++; inword=0; }    // close word, increase word count
            default:   { chars++; inword=1; }     // otherwise count as in-word character
        }
        ch=fgetc(stdin);
    }
    if (inword)  words++;     // close any final word without line change

    printf("%ld line(s), %ld word(s), %ld character(s)\n", lines, words, chars);

    return (0);
}
