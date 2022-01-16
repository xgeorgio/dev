/*
    FILE:           'bitblock.h'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Data type for handling bit blocks/streams/buffers
    VERSION:        1.1
    UPDATED:        16-Jan-2022/14:55

    DESCRIPTION:    A standard host type, typically unsigned long or
                    larger, is used similarly to the 'bitset' that is
                    available in C++ (STL) for static-length blocks.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#ifndef     _BITBLOCK_H
#define     _BITBLOCK_H

#include <memory.h>
#include <limits.h>


typedef     unsigned long       bitblock_host_t;   // for block host
typedef     unsigned short      bitblock_host_sz;  // for block size

typedef struct {
        bitblock_host_t     v;
        bitblock_host_sz    s;
                            }   bitblock;       // main bitblock type

extern const bitblock_host_sz bitblock_maxsz;   // length in bits (global)


#endif // _BITBLOCK_H
