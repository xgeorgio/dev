//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef     _DEFINES_H
#define     _DEFINES_H

#include <windows.h>

#define     MAX_LAYERS      20
#define     LOGFILENAME     "sifxw.log"
//#define     INIFILENAME     "fcdw.ini"
#define     UPDATE_EVERY    250

#define     VERSION_TAG         "SIFX - Sampled Image Feature Extractor 2.3 (Build: R000718-Win32-PPro)"

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

#define     xx      256
#define     yy      256
#define     N       256

#define     PAT_NONE            0
#define     PAT_ASP             1
#define     PAT_ASM             2
#define     PAT_BOTH            3

long ncombs( int n )
{
	return((long)n*(n-1)/2+(long)n*(n-1)*(n-2)/6+(long)n*(n-1)*(n-2)*(n-3)/24);
}


typedef struct stStatusInfo {
    char InpFileName[256];
    char LogFileName[256];
    char patFileName[256], imgFileName[256], datFileName[256];
    int  boxSize, datMode, gscale;
    FILE *inpfile, *logfile;
    POINT *sdat;
    int  iter, maxiter, imdata[xx*yy], drawImage;
    float nsbias, nslev;
    int  isInpFile, isLogFile, isNetwork; }  TStatusInfo;


HANDLE  ghInstance;
char    appName[] =  "SIFX - Sampled Image Feature Extractor";
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
void NEAR PASCAL greyscale( HDC, int, int );
int *imageplot( HDC, int, int, int* );

int __cdecl console_main( HWND hWnd );

#endif
