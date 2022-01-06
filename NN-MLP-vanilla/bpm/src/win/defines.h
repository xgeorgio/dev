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

#ifndef     _DEFINES_H
#define     _DEFINES_H

#include <windows.h>

#define     MAX_LAYERS      20
#define     LOGFILENAME     "bpm_net.log"
#define     INIFILENAME     "bpmw.ini"
#define     UPDATE_EVERY    250

#define     VERSION_TAG         "BPM - BackProp ANN Trainer version 4.2 (Build: R000717-BSA-Win32-PPro)"

typedef struct stStatusInfo {
    char InpFileName[256];
    char LogFileName[256];
    char trnFileName[256], valFileName[256], tstFileName[256];
    int  trnSize, valSize, tstSize, howmany;
    float lcoeff, mcoeff, goodvalptg, val_ptg;
    FILE *inpfile, *logfile;
    int  ivecSize, ovecSize, lay[MAX_LAYERS];
    int  iter, maxiter, *recptrn, nlay;
    int  *trnError, *valPtg, tstptrn, tstres[4];
    int  isInpFile, isLogFile, isNetwork, isOutput2; }  TStatusInfo;


HANDLE  ghInstance;
char    appName[] =  "BPM - BackProp ANN Trainer";
static OPENFILENAME ofn;
char szFileName[256] ="", szTitleName[256]="", appMesg[256]="";
TStatusInfo appStatus;
static int screenupdate;



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
int GetUpdateRate( void );


#endif
