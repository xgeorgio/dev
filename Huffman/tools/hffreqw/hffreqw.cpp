/*
    FILE:           'hffreqw.h'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Word frequency analyzer
    VERSION:        1.1
    UPDATED:        15-Jan-2022/14:55

    DESCRIPTION:    Main source file for file I/O.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#include "hffreqw.h"

//... Standard usage display ...
void usage( void )
{
    printf("USAGE:\n\thffreqw <filename>\n\n");
}


//...  Main program ...
int main(int argc, char* argv[])
{
    word_t         w;
    dictentry_t    wpair;
    dictmap_t      words;
    dictiter_t     wpos;

    printf("%s - %s\nversion %s %d.%d.%d.%d / %s / %s\n%s\n\n",
        APP_VERSION_NAME,APP_TAGINFO,APP_VERSION_STR,APP_VERSION_MAJOR,APP_VERSION_MINOR,
        APP_VERSION_BUILD,APP_VERSION_RELEASE,CPUID_ISET_STR,APP_VERSION_DATE,APP_COPYRIGHT);

    // main loop - begin input to make sure EOF is activated immediately if finished
    std::cin >> w;    // reads buffered words as automatic tokenizer
    while (!std::cin.eof())
    {
        //std::cout << "you entered: " << w;
        wpos=words.find(w);
        if (wpos == words.end())    // word not found in current dictionary
        {
            //std::cout << " (new)" << '\n';
            words.emplace(w,1);     // insert word, start its counter
        }
        else    // word already in the current dictionary
        {
            //std::cout << '\n';
            wpos->second++;
        }

        std::cin >> w;    // make sure to read before next EOF check
    }

    //std::cout << "dictionary contains: " << words.size() << '\n';
    // EOF reached, print the resulting dictionary
    for ( wpos=words.begin(); wpos!=words.end(); wpos++ )
    {
        std::cout << wpos->first << " " << wpos->second << '\n';
    }

    return (0);
}
