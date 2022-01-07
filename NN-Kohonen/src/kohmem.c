/*    ******************************************************************
      ***                                                            ***
      ***         Artificial Neural Networks - Kohonen model         ***
      ***         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~         ***
      ***                                                            ***
      ***           Harris Georgiou, AM:4  (csst9328@cs.uoi.gr)      ***
      ***     Dimitris Papadopoulos, AM:28 (csst9337@cs.uoi.gr)      ***
      ***                                                            ***
      ******************************************************************



        DESCRIPTION:
                        
            This file contains all source code needed to implement the
          basic structure of ANN graphs. General structures are used to
          define neurons, layers (1D/2D) and input/output layers (1D/2D).
          A specific network structure is defined for a simple Kohonen
          network of one input layer (1D), one normal layer (2D) and one
          output layer (2D). Input and output layers are not a collection
          of neurons (only normal layers are) and are just used to give
          a better access to inputs and outputs of the network.

            All structures are allocated dynamically (network structure
          sizes are specified at run-time), but after the network has
          been created it cannot change its structure. Internal neuron
          associations are not kept, because the layers are supposed to
          be fully linked (except the last with the output layer). 
          However, the real network behaviour is defined by the use and
          sequence of "fire" procedures of the network's neurons.


------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "kohnet.h"



/******************************************/


/* NNeuron */

int nneuron_init( NNeuron *neu, NCounter wnum, NWeight winit, NWeight lim, 
                                                NData (*tfun)(NData val) )
{
    NCounter    count;

    neu->w = (NWeight *)calloc( wnum, sizeof(NWeight) );
    if ( (neu->w)==NULL )
        return( ERR_ALLOC );
    else
    {
        for ( count=0; count<wnum; count++  )
            neu->w[count] = winit;
        neu->wnum = wnum;
        neu->lim = lim;
        neu->eval = (NData)0;
        neu->tfun = tfun;

        return( ERR_NONE );
    }
}

int nneuron_done( NNeuron *neu )
{
    if ( neu != NULL )
    {
        if ( (neu->w) != NULL )
        {
            free( neu->w );
            neu->w = NULL;
        }
    }

    return( ERR_NONE );
}



/* NLayerIO */

int nlayerio_init( NLayerIO *iolay, NCounter dnum )
{
    iolay->dat = (NData *)calloc( dnum, sizeof(NData) );
    if ( (iolay->dat)==NULL )
        return( ERR_ALLOC );
    else
    {
        iolay->dnum = dnum;
        
        return( ERR_NONE );
    }
}

int nlayerio_done( NLayerIO *iolay )
{
    if ( iolay != NULL )
    {
        if ( (iolay->dat) != NULL )
        {
            free( iolay->dat );
            iolay->dat = NULL;
        }
    }

    return( ERR_NONE );
}



/* NGrid */

int ngrid_init( NGrid *grd, NCounter nnumx, NCounter nnumy )
{
    NCounter    count, c;
    int         err;

    grd->neu = (NNeuron **)calloc( nnumx, sizeof(NNeuron *) );
    if ( (grd->neu)==NULL )
        return( ERR_ALLOC );
    else
    {
        err = 0;
        for ( count=0; (count<nnumx)&&(!err); count++  )
        {
            grd->neu[count] = (NNeuron *)calloc( nnumy, sizeof(NNeuron) );
            if ( (grd->neu[count])==NULL )
            {
                err = 1;
                for ( c=0; c<count; c++ )
                {
                    if ( (grd->neu[c]) != NULL )
                    {
                        free( grd->neu[c] );
                        grd->neu[c] = NULL;
                    }
                }
            }
        }
        if (err)
        {
            if ( (grd->neu) != NULL )
            {
                free( grd->neu );
                grd->neu = NULL;
            }

            return( ERR_ALLOC );
        }
        else
        {
            grd->nnumx = nnumx;
            grd->nnumy = nnumy;

            return( ERR_NONE );
        }
    }
}

int ngrid_done( NGrid *grd )
{
    NCounter    count;

    if ( grd != NULL )
    {
        if ( (grd->neu)!=NULL )
            for ( count=0; count<(grd->nnumx); count++ )
            {
                if ( (grd->neu[count]) != NULL )
                {
                    free( grd->neu[count] );
                    grd->neu[count] = NULL;
                }
            }
        if ( (grd->neu) != NULL )
        {
            free( grd->neu );
            grd->neu = NULL;
        }
    }

    return( ERR_NONE );
}



/* NGridIO */

int ngridio_init( NGridIO *iogrd, NCounter dnumx, NCounter dnumy )
{
    NCounter    count, c;
    int         err;

    iogrd->dat = (NData **)calloc( dnumx, sizeof(NData *) );
    if ( (iogrd->dat)==NULL )
        return( ERR_ALLOC );
    else
    {
        err = 0;
        for ( count=0; (count<dnumx)&&(!err); count++  )
        {
            iogrd->dat[count] = (NData *)calloc( dnumy, sizeof(NData) );
            if ( (iogrd->dat[count])==NULL )
            {
                err = 1;
                for ( c=0; c<count; c++ )
                {
                    if ( (iogrd->dat[c]) != NULL )
                    {
                        free( iogrd->dat[c] );
                        iogrd->dat[c] = NULL;
                    }
                }
            }
        }
        if (err)
        {
            if ( (iogrd->dat) != NULL )
            {
                free( iogrd->dat );
                iogrd->dat = NULL;
            }

            return( ERR_ALLOC );
        }
        else
        {
            iogrd->dnumx = dnumx;
            iogrd->dnumy = dnumy;

            return( ERR_NONE );
        }
    }
}

int ngridio_done( NGridIO *iogrd )
{
    NCounter    count;

    if ( iogrd != NULL )
    {
        if ( (iogrd->dat)!=NULL )
            for ( count=0; count<(iogrd->dnumx); count++ )
            {
                if ( (iogrd->dat[count]) != NULL )
                {
                    free( iogrd->dat[count] );
                    iogrd->dat[count] = NULL;
                }
            }
        if ( (iogrd->dat) != NULL )
        {
            free( iogrd->dat );
            iogrd->dat = NULL;
        }
    }

    return( ERR_NONE );
}



/* NKohNet */

int nkohnet_init( NKohNet *knet )
{
    NCounter    cx, cy, cw;
    int         err;
    NData	tval;


    /* allocate memory objects */
    err = nlayerio_init( &(knet->inp), knet->siz.inp );
    if (err)
    {
        /* nkohnet_done( knet ); */
        return( ERR_ALLOC );
    }
        
    err = ngrid_init( &(knet->hid), knet->siz.hidx, knet->siz.hidy );
    if (err)
    {
        /* nkohnet_done( knet ); */
        nlayerio_done( &(knet->inp) );
        return( ERR_ALLOC );
    }
    
    err = ngridio_init( &(knet->out), knet->siz.outx, knet->siz.outy );
    if (err) 
    {                
        /* nkohnet_done( knet ); */
        nlayerio_done( &(knet->inp) );
        ngrid_done( &(knet->hid) );
        return( ERR_ALLOC );
    }
    
    for ( cx=0; cx<(knet->hid.nnumx); cx++ )
        for ( cy=0; cy<(knet->hid.nnumy); cy++ )
            err = err || nneuron_init( &(knet->hid.neu[cx][cy]), knet->siz.inp,
                                        DEFWEIGHTVAL, DEFLIMITVAL, sigmoid );
    if (err)
    {                
        /*nkohnet_done( knet );*/
        return( ERR_ALLOC );
    }


    /* randomize all weights */
    /*randomize();*/
    for ( cx=0; cx<(knet->hid.nnumx); cx++ )
    {
        for ( cy=0; cy<(knet->hid.nnumy); cy++ )
        {
            /*for ( cw=0; cw<(knet->hid.neu[cx][cy].wnum); cw++ )
            {
                knet->hid.neu[cx][cy].w[cw] = (NData)rand()/RAND_MAX/10;
            }*/
	    
	    tval = ((NData)rand())/RAND_MAX;
	    knet->hid.neu[cx][cy].w[0] = tval;
	    tval = sqrt(1-tval*tval);
	    knet->hid.neu[cx][cy].w[1] = tval;

            /*knet->hid.neu[cx][cy].lim = (NData)rand()/RAND_MAX/10;*/
	    knet->hid.neu[cx][cy].lim = DEFLIMITVAL;
        }
    }

    return( ERR_NONE );
}


int nkohnet_done( NKohNet *knet )
{
    NCounter    cx, cy;

    
    if ( knet != NULL )
    {
        for ( cx=0; cx<(knet->hid.nnumx); cx++ )
            for ( cy=0; cy<(knet->hid.nnumy); cy++ )
                nneuron_done( &(knet->hid.neu[cx][cy]) );
    
        nlayerio_done( &(knet->inp) );
        ngrid_done( &(knet->hid) );
        ngridio_done( &(knet->out) );
    }

    return( ERR_NONE );
}



