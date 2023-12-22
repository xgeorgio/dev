/*
  +----------------------------------+
  |                                  |
  |   BPM - BackProp ANN Trainer     |
  |   --------------------------     |
  |                                  |
  |    Harris Georgiou (c) 2000      |
  |                                  |
  +----------------------------------+
*/

//................................................

#ifndef     __BPM_CONST_H__
#define     __BPM_CONST_H__

#include <stdio.h>
#include <math.h>

typedef    int          TCount;
typedef    float	    TWeight;
typedef	   float	    TValue;	

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

typedef struct {
	FILE   *fh;
	float  **inpvec, **outvec;
	TCount nvec, ivecsz, ovecsz, pos; }   Tmembuf;
	

#define		VERSION_TAG1	"Back-Propagation ANN Trainer, v4.12 - Universal version"
#define		VERSION_TAG2	"Build: R000619-BSA-Win32-PPro, Harris Georgiou (c) 2000."

#define     GOOD_VAL_PTG        ((TValue)95.0)
#define     MAX_ITERATIONS	    ((TCount)30000)
#define     LIM0		        ((TValue)0.3)
#define     LIM1		        ((TValue)0.7)

#define		DEFSAVNAME		    "bpm_net.txt"
#define     AUTOSAVE_EVERY	    200


#define     round(x)            floor(x+0.5)


#endif   /* __BPM_CONST_H__  */
