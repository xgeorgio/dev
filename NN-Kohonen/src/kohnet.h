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
                        
            This file contains constant and type definitions, used by the
          programs which implement the Kohonen ANN. The sigmoid function 
          is also defined, in order to be used as transfer function in
          network's neurons.



------------------------------------------------------------------------------
*/

#ifndef     _KOHMEM_H
#define     _KOHMEM_H

#include <math.h>

#define     ERR_NONE            0
#define     ERR_ALLOC           -1
#define     ERR_BOUND           -2
#define     ERR_FILE            -3

#define     DEFWEIGHTVAL        0.4
#define     DEFLIMITVAL         0.1


typedef     long                TCount;


typedef     double              NWeight;

typedef     int                 NCounter;

typedef     double              NData;


struct N_Neuron {
    NWeight     *w;
    NCounter    wnum;
    NWeight     lim;
    NData       eval;
    NData       (*tfun)( NData val );   };

typedef     struct N_Neuron     NNeuron;


struct N_LayerIO {
    NData       *dat;
    NCounter    dnum;   };

typedef     struct N_LayerIO    NLayerIO;


struct N_Grid {
    NNeuron     **neu;
    NCounter    nnumx;
    NCounter    nnumy;  };

typedef     struct N_Grid       NGrid;


struct N_GridIO {
    NData       **dat;
    NCounter    dnumx;
    NCounter    dnumy;  };

typedef     struct N_GridIO     NGridIO;


struct N_KNetDesc {
    NCounter    inp;
    NCounter    hidx;
    NCounter    hidy;
    NCounter    outx;
    NCounter    outy;   };

typedef     struct N_KNetDesc    NKNetDesc;


struct N_KohNet {
    NKNetDesc   siz;
    NLayerIO    inp;
    NGrid       hid;
    NGridIO     out;    };

typedef     struct N_KohNet     NKohNet;



/**********************************************/


NData sigmoid( NData val )
{
    return( (1/(1+exp(-val))) );
}


#endif

