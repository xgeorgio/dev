/*
    FILE:           'ver.h'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Setup global version tags
    VERSION:        1.1
    UPDATED:        16-Jan-2022/14:55

    DESCRIPTION:    Provide global versioning for the complete package
                    and access to configuration settings, as well as build
                    date/time and specifications for the executable.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#ifndef     _VERSION_H
#define     _VERSION_H

//... define main application version numbers ...
#define     APP_VERSION_NAME        "BITBLOCK"    //-> should be same as the main application
#define     APP_TAGINFO             "BitBlock testing module"
#define     APP_COPYRIGHT           "Harris Georgiou (c) 2022"

#define     APP_VERSION_MAJOR       1       //= development phase (completed)
#define     APP_VERSION_MINOR       1       //= functionality level (completed)
#define     APP_VERSION_RELEASE     2022    //= corrections & enhancements or release year (yyyy)
#define     APP_VERSION_BUILD       0116     //= build number or release date (mmdd)

#define     APP_VERSION_STR         "(release)"
#define     APP_VERSION_DATE        __DATE__    //-> should be same as the main application

//... check compile-time instruction set configuration ...
#if     (_M_IX86 == 300 )
    #define     CPUID_ISET_STR      "i386"
#elif   (_M_IX86 == 400 )
    #define     CPUID_ISET_STR      "i486"
#elif   (_M_IX86 == 500 )
    #define     CPUID_ISET_STR      "iPentium"
#elif   (_M_IX86 == 600 )
    #define     CPUID_ISET_STR      "iPentiumPro"
#else
    #define     CPUID_ISET_STR      "(Generic)"
#endif

#endif      // _VERSION_H
