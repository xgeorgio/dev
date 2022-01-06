//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_CONST_H
#define		_CONST_H


#define		VERSION_VER		"2.3"
#define		VERSION_REL		"R000711"

#define     BMP_header      1078

#define     xx      256
#define     yy      256

#define		GSCALE_VALUES		256
//#define		GSCALE_COLORS		16

#ifndef		MAX_CLASS
#define		MAX_CLASS			2
#endif

#define		MAX_BOXSIZE			10
#define		MAX_FEATURE			20
#define		MAX_LINESIZE		1000

//#define		PATFILE_PREFX		"sfp-"
#define		TMPFILENAME			"fcd-work.tmp"
#define		AUX0_FILENAME		"fcd-all.dat"
#define		AUX1_FILENAME		"fcd-sel.dat"
#define		AUX2_FILENAME		"fcd-mdc.dat"

#define		LOGMINX				(double)0.0001
//#define		MINFEATSELECT		4
//#define		ATLEASTSELECT		2

//#define		DEBUG_PRINT			1

#define     PAT_NONE            0
#define     PAT_ASP             1
#define     PAT_ASM             2


#define		round(x)		floor(x+0.5)

#define		max(x,y)		((x>y)?x:y)

#define		min(x,y)		((x<y)?x:y)


#endif
