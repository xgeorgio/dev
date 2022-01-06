//----------------------------------------------------
//  
//   File: Multi-Layer Perceptron ANN executor, v1.7
//
//----------------------------------------------------
//                Harris Georgiou, 2000.
//----------------------------------------------------


#ifndef __MLP_CONST_H__
#define __MLP_CONST_H__

typedef    int		TCount;		//--> Note: sizeof(int)=4 for Win32.

typedef    float	TWeight;

typedef	   float	TValue;	

typedef struct {
	TWeight	*w;
	TWeight *wprev;
	TCount	wnum;
    TWeight h;
	TValue	eval; }		TNeuron;

typedef struct {
	TNeuron	*neu;
	TCount	nnum; }		TLayer;

typedef struct {
	TLayer	*lay;
	TCount	lnum; }		TNet;


#define GOOD_PTG	((TValue)0.95)
#define LIM0		((TValue)0.3)
#define LIM1		((TValue)0.7)

#define SCREENSTEP	100


//#define         MAXLAYERNUM             20
//#define         MAXNEURONNUM            200
//#define         MAXWEIGHTNUM            MAXNEURONNUM

#define		DEFSAVNAME		"bpm_net.txt"


TCount  ivecsz=0, ovecsz=0, nlayers=0, *neurons;

//TValue  *ivec, *ovec;
TCount  ivecs=0;


#define			MODE_PRED		1
#define			MODE_EVAL		2

int     mode=MODE_PRED;
  

#define			SIGMOID_STD			1
#define			SIGMOID_TANH		2

int     sigmoid_sel=SIGMOID_STD;


#define			OUTP_QUANT		1
#define			OUTP_ABS		2
#define			OUTP_MAXP		3

int		output_sel=OUTP_ABS;

float   mean_match=0.0;

//int		outsz=0;

#endif   /* __MLP_CONST_H__  */
