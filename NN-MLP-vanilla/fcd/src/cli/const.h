//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_CONST_H
#define		_CONST_H


#define		VERSION_VER		"2.2"
#define		VERSION_REL		"R000617"

//#define     BMP_header      1078

//#define     xx      256
//#define     yy      256

//#define		GSCALE_VALUES		256
//#define		GSCALE_COLORS		16

//#define		MAX_BOXSIZE			10
#define		MAX_FEATURE			3
#define		MAX_LINESIZE		1000
//#define		MAX_PATTERNS		1000

//#define		N					MAX_FEATURE
//#define		FEATURE_COMB2		(int)(N*(N-1)/2)
//#define		FEATURE_COMB3		(int)(N*(N-1)*(N-2)/6)
//#define		FEATURE_COMB4		(int)(N*(N-1)*(N-2)*(N-3)/24)
//#define		FEATURE_COMBS		(int)(FEATURE_COMB2+FEATURE_COMB3+FEATURE_COMB4)
//#define		MAX_COMB_SIZE		(int)(20000)

//#define		PATFILE_PREFX		"sfp-"
#define		TMPFILENAME			"fcd-work.tmp"
#define		AUX0_FILENAME		"fcd-all.dat"
#define		AUX1_FILENAME		"fcd-sel.dat"
#define		AUX2_FILENAME		"fcd-mdc.dat"

//#define		LOGMINX				(double)0.0001
#define		MINFEATSELECT		1
#define		ATLEASTSELECT		1

//#define		DEBUG_PRINT			1

#define		round(x)		floor(x+0.5)

#define		max(x,y)		((x>y)?x:y)

#define		min(x,y)		((x<y)?x:y)


long ncombs( int n )
{
	return((long)n*(n-1)/2+(long)n*(n-1)*(n-2)/6+(long)n*(n-1)*(n-2)*(n-3)/24);
}


#endif
