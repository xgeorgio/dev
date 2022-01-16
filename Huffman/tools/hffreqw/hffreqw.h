/*
    FILE:           'hffreqw.h'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Word frequency analyzer
    VERSION:        1.1
    UPDATED:        15-Jan-2022/14:55

    DESCRIPTION:    Main source file for file I/O.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#ifndef     _HFFREQW_H
#define     _HFFREQW_H

#include <stdio.h>
#include <map>
#include <string>
#include <bits/stdc++.h>
#include <iostream>
#include "ver.h"

typedef    std::string      word_t;
typedef    unsigned long    counter_t;

typedef    std::pair< word_t, counter_t >    dictentry_t;
typedef    std::map< word_t, counter_t >     dictmap_t;
typedef    std::map< word_t, counter_t >::iterator    dictiter_t;

#endif
