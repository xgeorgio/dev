/*
    FILE:           'bitblock.c'
    PACKAGE:        Huffman coding trees

    PURPOSE:        Data type for handling bit blocks/streams/buffers
    VERSION:        1.1
    UPDATED:        16-Jan-2022/14:55

    DESCRIPTION:    A standard host type, typically unsigned long or
                    larger, is used similarly to the 'bitset' that is
                    available in C++ (STL) for static-length blocks.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#include "bitblock.h"

// define global constant for block length in bits
const bitblock_host_sz bitblock_maxsz=sizeof(bitblock_host_t)*CHAR_BIT;


// get max host type length in bits
bitblock_host_sz bitblock_maxbits( void )
{
    return (bitblock_maxsz);
}


// direct copy from src bitblock
void bitblock_copy( bitblock *bb, const bitblock *src )
{
    if (bb && src)
    {
        bb->v = src->v;
        bb->s = src->s;
    }
}


// copy from a standard host value (binary)
void bitblock_from_val( bitblock *bb, const bitblock_host_t val )
{
    if (bb)
    {
        bb->v = val;
        bb->s = bitblock_maxsz;
    }
}


// convert to standard host value (binary)
bitblock_host_t bitblock_to_val( const bitblock *bb )
{
    if (bb)
        return (bb->v);
    else
        return (0);
}


// initialize with zero
void bitblock_reset( bitblock *bb )
{
    bitblock_from_val(bb,(bitblock_host_t)0);
}


// set a specific bit to 1 or 0
int bitblock_set_bit( bitblock *bb, const bitblock_host_sz pos, const unsigned short val )
{
    //bitblock_host_t  m=0;

    if ((pos>=0) && (pos<bitblock_maxsz))
    {
        //m = (bitblock_host_t)1 << pos;
        //bb->v = ((val) ? (bb->v | m) : (bb->v & ~m) );
        bb->v = ( (val) ? (bb->v | ((bitblock_host_t)1 << pos)) : (bb->v & ~((bitblock_host_t)1 << pos)) );
        return (1);
    }
    else
        return (-1);
}


// get the value of a specific bit
int bitblock_get_bit( const bitblock *bb, const bitblock_host_sz pos )
{
    //bitblock_host_t  m=0;

    if ((pos>=0) && (pos<bitblock_maxsz))
    {
        //m = (bitblock_host_t)1 << pos;
        //return (bb->v & m)
        return ((bb->v & ((bitblock_host_t)1 << pos)) >> pos);
    }
    else
        return (-1);
}


// flip the value of a specific bit
int bitblock_flip_bit( bitblock *bb, const bitblock_host_sz pos )
{
    return (bitblock_set_bit(bb,pos,~bitblock_get_bit(bb,pos)));
}


// reverse the order of characters in a standard C string,
// can be used to convert LSB/MSB string representations
static char * mirror_cstr( char *str, bitblock_host_sz len )
{
    bitblock_host_sz  c=0, mx;
    char   t;

    if (str)
    {
        mx=(bitblock_host_sz)len/2;
        while (c<=mx)
        {
            t=str[c];
            str[c]=str[(len-1)-c];
            str[(len-1)-c]=t;
            c++;
        }
    }

    return(str);
}


//-----  main testing module  -----

#include <stdio.h>

char * bitblock_to_cstr( const bitblock *bb, char *str )
{
    bitblock_host_sz  c=0;
    bitblock_host_t   t;

    if (bb && str)
    {
        t = bb->v;
        printf("inside to_cstr: (bb->s:%d) ",bb->s);
        while (c < bb->s)
        {
            str[c] = (t & (bitblock_host_t)1)?'1':'0';
            printf("%c",str[c]);
            t = t>>1;
            c++;
        }
        str[c]='\0';
        printf("\n");
        mirror_cstr(str,c);
        return(str);
    }
    else
        return(NULL);
}
