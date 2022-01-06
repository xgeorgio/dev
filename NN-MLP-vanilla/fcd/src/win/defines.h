//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef     _DEFINES_H
#define     _DEFINES_H

#include <windows.h>

#define     MAX_LAYERS      20
#define     LOGFILENAME     "fcdw.log"
//#define     INIFILENAME     "fcdw.ini"
#define     UPDATE_EVERY    250

#define     VERSION_TAG         "FCD - Feature Classifier Design 2.2 (Build: R000717-Win32-PPro)"

#define		MAX_FEATURE			20
#define		MAX_LINESIZE		1000

#define		TMPFILENAME			"fcd-work.tmp"
#define		AUX0_FILENAME		"fcd-all.dat"
#define		AUX1_FILENAME		"fcd-sel.dat"
#define		AUX2_FILENAME		"fcd-mdc.dat"

#define		MINFEATSELECT		4
#define		ATLEASTSELECT		2

#define		round(x)		floor(x+0.5)

//#define		max(x,y)		((x>y)?x:y)

//#define		min(x,y)		((x<y)?x:y)


long ncombs( int n )
{
	return((long)n*(n-1)/2+(long)n*(n-1)*(n-2)/6+(long)n*(n-1)*(n-2)*(n-3)/24);
}


typedef struct stStatusInfo {
    char InpFileName[256];
    char LogFileName[256];
    char patFileName[256], mdcFileName[256], distmode[10];
    int  patSize, ivecSize, svecSize, mvecSize;
    FILE *inpfile, *logfile;
    float   tlimit, bestptg, *valPtg;
    int  iter, maxiter, minsel, fvec[MAX_FEATURE], fvecsz;
    int  tstptrn, tstres[4];
    int  isInpFile, isLogFile, isNetwork, isOutput2; }  TStatusInfo;


HANDLE  ghInstance;
char    appName[] =  "FCD - Feature Classifier Design";
static OPENFILENAME ofn;
char szFileName[256] ="", szTitleName[256]="", appMesg[256]="";
TStatusInfo appStatus;
//static int screenupdate;


LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT WINAPI AboutDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL PopFileOpenDlg(HWND, PSTR, PSTR);
void PopFileInitialize(HWND hwnd);

void DrawClientWindow( HWND );
void DrawClientMessages( HWND );
void ResetStatus( TStatusInfo* , BOOL );
BOOL isReady( TStatusInfo* );
void ShowMessage( HWND, char* );
void DrawMessage( HDC, char* );
void LogMessage( TStatusInfo* , char* );
//int GetUpdateRate( void );

int __cdecl console_main( HDC hdc );


#endif
