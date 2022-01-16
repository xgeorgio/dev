#define     NULL     0

typedef     unsigned long       bitblock_host_t;
typedef     unsigned short      bitblock_host_sz;

typedef struct {
        bitblock_host_t     v;
        bitblock_host_sz    s;
                            }   bitblock;

const bitblock_host_sz bitblock_maxsz=sizeof(bitblock_host_t)*8;


bitblock_host_sz bitblock_maxbits( void )
{
    return (bitblock_maxsz);
}


void bitblock_copy( bitblock *bb, const bitblock *src )
{
    if (bb && src)
    {
        bb->v = src->v;
        bb->s = src->s;
    }
}


void bitblock_from_val( bitblock *bb, const bitblock_host_t val )
{
    if (bb)
    {
        bb->v = val;
        bb->s = bitblock_maxsz;
    }
}


bitblock_host_t bitblock_to_val( const bitblock *bb )
{
    if (bb)
        return (bb->v);
    else
        return (0);
}


void bitblock_reset( bitblock *bb )
{
    bitblock_from_val(bb,(bitblock_host_t)0);
}


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


int bitblock_flip_bit( bitblock *bb, const bitblock_host_sz pos )
{
    return (bitblock_set_bit(bb,pos,~bitblock_get_bit(bb,pos)));
}


static char * flip_cstr( char *str, bitblock_host_sz len )
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


char * bitblock_to_cstr( const bitblock *bb, char *str )
{
    bitblock_host_sz  c=0;
    bitblock_host_t   t;

    if (bb && str)
    {
        t = bb->v;
        while (c < bb->s)
        {
            str[c] = (t & (bitblock_host_t)1)?'1':'0';
            t = t>>1;
            c++;
        }
        str[c]='\0';
        flip_cstr(str,c);
        return(str);
    }
    else
        return(NULL);
}


void setup() {
  // put your setup code here, to run once:
  

}


bitblock  bb;
int       pos=3;
char      str[33];
bitblock_host_t  val=0;

void loop() {
    pos=bitblock_maxbits();
    bitblock_reset(&bb);
    pos=bitblock_get_bit(&bb,3);
    bitblock_set_bit(&bb,pos,1);
    bitblock_get_bit(&bb,5);
    val=(bitblock_host_t)bitblock_to_val(&bb);
    bitblock_to_cstr(&bb,str);
}
