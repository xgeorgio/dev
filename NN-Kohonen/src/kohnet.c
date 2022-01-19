/*    ******************************************************************
      ***                                                            ***
      ***         Artificial Neural Networks - Kohonen model         ***
      ***         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~         ***
      ***                                                            ***
      ***                                                            ***
      ******************************************************************



        DESCRIPTION:
                        
            This file contains all source code needed to implement the
          functionality and data process in a Kohonen ANN.
          
            There are implementations for all main procedures that are
          used for training and using the network. Also, there are 
          parametric implementations for learning and distance coefficients,
          "low-level" neuron fire procedures and abstract ANN stimulation
          and training procedures. The first one is model-independent,
          while the second are model-specific.



------------------------------------------------------------------------------
*/

#include <float.h>
#include <math.h>

#include "kohnet.h"
#include "koh.h"



/* Definition of parameters' default values and limits */

#define         DEFLCOEFVAL         -0.000001
#define         DEFLRNT1VAL         100
#define         DEFLRNT2VAL         200
#define         DEFLRNK1VAL         0.75
#define         DEFLRNK2VAL         0.30

#define         DEFDCOEFVAL         1
#define         DEFDIST1VAL         10
#define         DEFDIST2VAL         1

#define         INITDISTVAL         DBL_MAX
#define         DEFMINOUTVAL        DBL_MIN

#define		CONSC_COEFF	2
#define		C_COEFF_VAL	0.5



/* Global parameter variables (may be changed by the main program) */

TCount      learn_T1 = DEFLRNT1VAL;
TCount      learn_T2 = DEFLRNT2VAL;
NData       learn_K1 = DEFLRNK1VAL;
NData       learn_K2 = DEFLRNK2VAL;
NCounter    dist_D1  = DEFDIST1VAL;
NCounter    dist_D2  = DEFDIST2VAL;

NData		p_coeff[HDIMX][HDIMY];
NData		dmax_coeff[HDIMX][HDIMY];
const NData	c_coeff = C_COEFF_VAL;

TCount		cycle;




/* Learning coefficient - parametric function */

NData learn_coef( TCount cycle )
{
    NData   learn;


    if ( (0 <= cycle) && (cycle < learn_T1) )
    {
        learn = learn_K1*(1-(double)cycle/learn_T1);
        if ( learn < learn_K2 )     /* make sure the function is always decreasing */
            learn = learn_K2;
    }
    else if ( (learn_T1 <= cycle) && (cycle <= learn_T2) )
        learn = learn_K2*(1-(double)(cycle-learn_T1)/(learn_T2-learn_T1));
    else
        learn = DEFLCOEFVAL;        /* error situation */

    return( learn );
}



/* Distance coefficient - parametric function */

NCounter dist_coef( TCount cycle )
{
    NCounter    dist;


    if ( (0 <= cycle) && (cycle <= learn_T1) )
    {
        dist = (NCounter)( dist_D1*(1-(double)cycle/learn_T1) );
        if ( dist < dist_D2 )     /* make sure the function is always decreasing */
            dist = dist_D2;
    }
    else if ( (learn_T1 <= cycle) && (cycle <= learn_T2) )
        dist = (NCounter)dist_D2;
    else
        dist = DEFDCOEFVAL;        /* error situation */
        
    return( dist );
}







NData conscience_coeff( TCount cycle )
{
    return( CONSC_COEFF*(1-((NData)cycle)/learn_T2) );
}

void init_conscience( NKohNet *knet )
{
    NCounter   cx, cy;

    for ( cx=0; cx<(knet->hid.nnumx); cx++ )
	for ( cy=0; cy<(knet->hid.nnumy); cy++ )
	{
	    p_coeff[cx][cy] = 1/((NData)(knet->hid.nnumx)*(knet->hid.nnumy));
	    dmax_coeff[cx][cy] = 0;
	}
}

NData conscience( NKohNet *knet, NCounter nx, NCounter ny, NData curoutval, TCount cycle )
{
    NData	b_coeff;


    b_coeff = conscience_coeff(cycle) + (1-(knet->hid.nnumx)*(knet->hid.nnumy)*p_coeff[nx][ny])*dmax_coeff[nx][ny];
    if (curoutval>dmax_coeff[nx][ny])
	dmax_coeff[nx][ny] = curoutval;
    p_coeff[nx][ny] = p_coeff[nx][ny]*(1-c_coeff)+c_coeff;

    return( b_coeff );
}




/* Select node of maximum output value, as area center */

int regm_center( NKohNet *knet, NData *input, NCounter *nminx, NCounter *nminy )
{
    NCounter    cx, cy, cw;
    NData       dval, tval;

    
    *nminx = 0;
    *nminy = 0;
    dval = INITDISTVAL;
    for ( cx=0; cx<(knet->hid.nnumx); cx++ )
    {
        for ( cy=0; cy<(knet->hid.nnumy); cy++ )
        {
            tval = (NData)0;
            /*printf("CALCULATING DIFF(%d,%d)\n",cx,cy);*/
            for ( cw=0; cw<(knet->hid.neu[cx][cy].wnum); cw++ )
            {
                tval = tval + (input[cw]-(knet->hid.neu[cx][cy].w[cw]))
                                *(input[cw]-(knet->hid.neu[cx][cy].w[cw]));
            }
	    tval = sqrt(tval);

	    /*tval = tval + conscience(knet,cx,cy,tval,cycle);*/

            if ( tval < dval )
            {
                dval = tval;
                *nminx = cx;
                *nminy = cy;
            }
            /*printf("DIFF(%d,%d)=%g\n",cx,cy,tval);*/
        }
    }

    return( ERR_NONE );
}



/* Check if a node is included in a stimulated area */

int regm_isnear( /*NKohNet *knet,*/ NCounter selx, NCounter sely, NCounter testx,
                        NCounter testy, TCount cycle )
{
    NCounter    dist;

    
    dist = dist_coef( cycle );
    
    /* Note: if "NCounter" is unsigned type, "(long)" type-conversion
             must be used to prevent underflow in the following subtructions
             that can cause false test results !!! */
    
    if ( ( (labs((long)testx-(long)selx) <= ((NCounter)dist/2)) )
            && ( (labs((long)testy-(long)sely) <= ((NCounter)dist/2)) ) )
        return(1);
    else
        return(0);
}



/* Update all nodes' weights in a stimulated area */

int regm_update( NKohNet *knet, NData *input, TCount cycle )
{
    NCounter    nselx, nsely, cx, cy, cw;
    NWeight     *tw;


    regm_center( knet, input, &nselx, &nsely );
    /*printf("NEW CENTER: (%d,%d)\n",nselx,nsely);*/
    if ( (0 <= cycle) && (cycle <= learn_T1) )  /* not neccessary, but speeds up the process */
    {
        for ( cx=0; cx<(knet->hid.nnumx); cx++ )
        {
            for ( cy=0; cy<(knet->hid.nnumy); cy++ )
            {
                if ( regm_isnear( /*knet,*/ nselx,nsely,cx,cy,cycle) )   /* updates the center too */
                {
                    /*printf("UPDATING NEURON(%d,%d)\n",cx,cy);*/
                    for ( cw=0; cw<(knet->hid.neu[cx][cy].wnum); cw++ )
                    {
                        tw = &(knet->hid.neu[cx][cy].w[cw]);
                        (*tw) =  (*tw) + learn_coef(cycle)*(input[cw]-(*tw));
                    }
                }
            }
        }
    }
    else if ( (learn_T1 <= cycle) && (cycle <= learn_T2) )
    {
        for ( cw=0; cw<(knet->hid.neu[nselx][nsely].wnum); cw++ )
        {
            tw = &(knet->hid.neu[nselx][nsely].w[cw]); 
            (*tw) =  (*tw) + learn_coef(cycle)*(input[cw]-(*tw));
        }
    }
    /* else, do nothing */

    return( ERR_NONE );
}



/* Fire a node and calculate output value */

int nneuron_fire( NNeuron *neu, NData *input )
{
    NCounter    count;
    NData       sum;

    for ( count=0,sum=0; count<(neu->wnum); count++ )
        sum = sum + (neu->w[count])*(input[count]);
    sum = sum + (neu->lim);
    neu->eval = (neu->tfun)(sum);

    return( ERR_NONE );
}



/* Fire the entire network (Kohonen) and calculate output values */

int nkohnet_fire( NKohNet *knet, NData *input )
{
    NCounter    count, cx, cy;

    if ( ((knet->hid.nnumx)==(knet->out.dnumx))
            && ((knet->hid.nnumy)==(knet->out.dnumy)) )
    {
        for ( count=0; count<(knet->inp.dnum); count++ )
            knet->inp.dat[count] = input[count];

        for ( cx=0; cx<(knet->hid.nnumx); cx++ )
            for ( cy=0; cy<(knet->hid.nnumy); cy++ )
            {
                nneuron_fire( &(knet->hid.neu[cx][cy]), (knet->inp.dat) );
                knet->out.dat[cx][cy] = knet->hid.neu[cx][cy].eval;
            }

        return( ERR_NONE );
    }
    else
        return( ERR_BOUND );
}



/* Stimulate network (Kohonen) and calculate selected output node */

int nkohnet_eval( NKohNet *knet, NData *input, NCounter *centerx, NCounter *centery )
{
    NCounter    cx, cy;
    NData       maxval;
    int         fireres;

    if ( ((knet->hid.nnumx)==(knet->out.dnumx))
            && ((knet->hid.nnumy)==(knet->out.dnumy)) )
    {
        fireres = nkohnet_fire( knet, input );        
        if ( fireres != ERR_NONE )
            return( fireres );
        
        maxval = DEFMINOUTVAL;
        *centerx = 0;
        *centery = 0;

        for ( cx=0; cx<(knet->hid.nnumx); cx++ )
            for ( cy=0; cy<(knet->hid.nnumy); cy++ )
            {   
                if ( (knet->out.dat[cx][cy]) > maxval )
                {
                    maxval = (knet->out.dat[cx][cy]);
                    *centerx = cx;
                    *centery = cy;
                }
            }

        return( ERR_NONE );
    }
    else
        return( ERR_BOUND );
}




int nkohnet_dump( NKohNet *knet, TCount cycle, FILE *dumpfile )
{
    NCounter	cx, cy, cw;

    fprintf(dumpfile,"\nKohonen Network - Status at cycle #%ld\n",cycle);
    for ( cx=0; cx<(knet->hid.nnumx); cx++ )
	for ( cy=0; cy<(knet->hid.nnumy); cy++ )
	{
	    fprintf(dumpfile,"neuron[%dx,%dy] = ",cx,cy);
	    for ( cw=0; cw<(knet->hid.neu[cx][cy].wnum); cw++ )
	    {
		fprintf(dumpfile," %f ",(knet->hid.neu[cx][cy].w[cw]));
	    }
	    fprintf(dumpfile,"\n");
	}

    return(0);
}

