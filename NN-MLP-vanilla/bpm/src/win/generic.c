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

#include <windows.h>
#include <stdio.h>
#include <commdlg.h>
#include <math.h>
#include "generic.h"
#include "defines.h"


int __cdecl console_main( HDC hdc );


int PASCAL WinMain( HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszCmdLine,
    int nCmdShow )
{
   WNDCLASS wc;
   MSG msg;
   HWND hWnd;

   if( !hPrevInstance )
   {
      wc.lpszClassName = "GenericAppClass";
      wc.lpfnWndProc = MainWndProc;
      wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
      wc.hInstance = hInstance;
      wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
      wc.hCursor = LoadCursor( NULL, IDC_ARROW );
      //wc.hbrBackground = (HBRUSH)( COLOR_WINDOW+1 );
      wc.hbrBackground = GetStockObject(BLACK_BRUSH);
      wc.lpszMenuName = "GenericAppMenu";
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;

      RegisterClass( &wc );
   }

   ghInstance = hInstance;
   
   hWnd = CreateWindow( "GenericAppClass",
      appName,
      //WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
      WS_OVERLAPPEDWINDOW,
      0,
      0,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL
   );

   //ShowWindow( hWnd, nCmdShow );
   ShowWindow( hWnd, SW_MAXIMIZE );

   while( GetMessage( &msg, NULL, 0, 0 ) ) {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }

   return msg.wParam;
}


LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam,
   LPARAM lParam )
{
   switch( msg ) {

      case WM_CREATE:
         PopFileInitialize(hWnd);
         ResetStatus(&appStatus,TRUE);
         screenupdate=GetUpdateRate();
	     break;
	
      case WM_PAINT:
          DrawClientWindow(hWnd);
          break;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDM_ABOUT:
               DialogBox(ghInstance,"AboutDlg",hWnd,(DLGPROC)AboutDlgProc);
            break;
		        
            case IDM_OPEN:
	            if(PopFileOpenDlg(hWnd, szFileName, szTitleName))
				{
                    ResetStatus(&appStatus,TRUE);
                    if ((appStatus.inpfile=fopen(szFileName,"r"))==NULL)
                    {
                        MessageBox(NULL, "Unable to open input file.", appName, MB_ICONEXCLAMATION);
                        appStatus.isInpFile=FALSE;
                    }
                    else
                    {
                        strcpy(appStatus.InpFileName,szFileName);
                        appStatus.isInpFile=TRUE;
                        DrawMessage(hWnd,"CFG file opened");
                    }

                    if ((appStatus.logfile=fopen(LOGFILENAME,"w+"))==NULL)
                    {
                        MessageBox(NULL, "Unable to create log file.", appName, MB_ICONEXCLAMATION);
                        appStatus.isLogFile=FALSE;
                    }
                    else
                    {
                        appStatus.isLogFile=TRUE;
                        LogMessage(&appStatus,VERSION_TAG);
                        LogMessage(&appStatus,"\n");
                    }

                    // place file read code here

                    MessageBox(NULL, "Files opened.", appName, MB_ICONINFORMATION);
				} 
       	        break;

	        case IDM_START:
                if (isReady(&appStatus))
                {
                    if (MessageBox(NULL, "Start processing?", appName, MB_ICONEXCLAMATION|MB_OKCANCEL)==IDOK)
                    {
                        //place process code here
                        SetCursor(LoadCursor(NULL,IDC_WAIT));
                        console_main(hWnd);
                        SetCursor(LoadCursor(NULL,IDC_ARROW));
                        ResetStatus(&appStatus,FALSE);
                        
                        MessageBox(NULL, "Process completed!", appName, MB_ICONINFORMATION);                        
                    }
                }
       	        break;

            case IDM_EXIT:  ResetStatus(&appStatus,TRUE); break;
         }

	    if (wParam==IDM_EXIT)
		    SendMessage(hWnd,WM_SYSCOMMAND,SC_CLOSE,0L); 
	        
        break;

      case WM_DESTROY:
         PostQuitMessage( 0 );
         break;

      default:
         return( DefWindowProc( hWnd, msg, wParam, lParam ));
   }

   return 0;
}


LRESULT CALLBACK AboutDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   switch( uMsg ) {
      case WM_INITDIALOG:
         return TRUE;
      case WM_COMMAND:
         switch( wParam ) {
            case IDOK:
               EndDialog( hDlg, TRUE );
               return TRUE;
         }
      break;
   }

   return FALSE;
}


void PopFileInitialize(HWND hwnd)
{
    static char szFilter[] = "Input Files (*.inp)\0*.inp\0"  \
                              "All Files (*.*)\0*.*\0\0";

    ofn.lStructSize       = sizeof (OPENFILENAME);
    ofn.hwndOwner         = hwnd;
    ofn.hInstance         = NULL;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = NULL;          // Set in Open and Close functions
    ofn.nMaxFile          = 256;
    ofn.lpstrFileTitle    = NULL;          // Set in Open and Close functions
    ofn.nMaxFileTitle     = 256;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = NULL;
    ofn.Flags             = 0;             // Set in Open and Close functions
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "inp";
    ofn.lCustData         = 0L;
    ofn.lpfnHook          = NULL;
    ofn.lpTemplateName    = NULL;
}


BOOL PopFileOpenDlg( HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName )
{
    ofn.hwndOwner         = hwnd ;
    ofn.lpstrFile         = pstrFileName ;
    ofn.lpstrFileTitle    = pstrTitleName ;
    ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

    return(GetOpenFileName(&ofn));
}


//***********************************************************

void DrawClientWindow( HWND hWnd )
{
   static PAINTSTRUCT ps;
   static HDC hdc;
   static RECT r;
   static HPEN pen;
   static HBRUSH brush;
   static char strBuffer[256];
   static int c, cx, cy, scrstep, minleft=10, maxleft=450, gwidth;


   // initialize drawing
   hdc = BeginPaint( hWnd, &ps );
   GetClientRect(hWnd, &r); 

   // typical drawing output in the client window
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0x00,0x7F,0x00)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0x00,0x7F,0x00)));

   RoundRect(hdc,r.left,30,r.right,40,10,10);

   Rectangle(hdc,500,40,530,r.bottom-100);
   RoundRect(hdc,500,r.bottom-110,530,r.bottom-45,10,10);
   Ellipse(hdc,490,30,510,50);
   Ellipse(hdc,520,30,540,50);

   RoundRect(hdc,520,200,r.right,210,10,10);
   Ellipse(hdc,520,190,540,210);
   Ellipse(hdc,520,200,540,220);

   MoveToEx(hdc,minleft,200,NULL);  LineTo(hdc,maxleft,200);
   MoveToEx(hdc,minleft,200,NULL);  LineTo(hdc,minleft,100);
   MoveToEx(hdc,minleft,330,NULL);  LineTo(hdc,maxleft,330);
   MoveToEx(hdc,minleft,330,NULL);  LineTo(hdc,minleft,230);
   MoveToEx(hdc,minleft,460,NULL);  LineTo(hdc,maxleft,460);
   MoveToEx(hdc,minleft,460,NULL);  LineTo(hdc,minleft,360);

   DeleteObject(pen);
   DeleteObject(brush);
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0x00,0x00,0x00)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0x00,0x00,0x00)));
   
   Ellipse(hdc,480,40,500,60);
   Ellipse(hdc,530,40,550,60);

   Ellipse(hdc,530,180,550,200);
   Ellipse(hdc,530,210,550,230);

   // typical text outout in the client window
   SetBkMode(hdc,TRANSPARENT);
   SetTextColor(hdc,RGB(0x00,0xAF,0x00));

   sprintf(strBuffer,"%s",VERSION_TAG);
   TextOut(hdc,10,10,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Graphs");
   TextOut(hdc,220,45,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Configuration");
   TextOut(hdc,620,45,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Results");
   TextOut(hdc,640,215,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"training error");
   TextOut(hdc,200,205,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"recognized");
   TextOut(hdc,200,335,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"%%validation");
   TextOut(hdc,200,465,strBuffer,strlen(strBuffer));
   
   // draw graphs
   DeleteObject(pen);
   DeleteObject(brush);
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0x00,0xFF,0x00)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0x00,0xFF,0x00)));
   SetTextColor(hdc,RGB(0xAF,0xCF,0xAF));

   sprintf(strBuffer,"CFG file: '%s'",appStatus.InpFileName);
   TextOut(hdc,550,65,strBuffer,strlen(strBuffer));  

   if (appStatus.isNetwork) {
   sprintf(strBuffer,"Training set: '%s' (size=%d)",appStatus.trnFileName,appStatus.trnSize);
   TextOut(hdc,550,80,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Validation set: '%s' (size=%d)",appStatus.valFileName,appStatus.valSize);
   TextOut(hdc,550,95,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Training set: '%s' (size=%d)",appStatus.tstFileName,appStatus.tstSize);
   TextOut(hdc,550,110,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Network: ");
   if (appStatus.ivecSize)  sprintf(strBuffer,"%s[%d]-",strBuffer,appStatus.ivecSize);
   for ( c=0; c<appStatus.nlay; c++ )
       sprintf(strBuffer,"%s%d-",strBuffer,appStatus.lay[c]);
   if (appStatus.ovecSize)  sprintf(strBuffer,"%s[%d]",strBuffer,appStatus.ovecSize);
   TextOut(hdc,550,125,strBuffer,strlen(strBuffer));  

   sprintf(strBuffer,"Coefficients:  l=%4.2f m=%4.2f",appStatus.lcoeff,appStatus.mcoeff);
   TextOut(hdc,550,140,strBuffer,strlen(strBuffer));  
   sprintf(strBuffer,"Max.Iterations:  %ld epochs",appStatus.maxiter);
   TextOut(hdc,550,155,strBuffer,strlen(strBuffer));  
   sprintf(strBuffer,"Good.Val.Ptg:  %5.2f",appStatus.goodvalptg);
   TextOut(hdc,550,170,strBuffer,strlen(strBuffer));  
   
   TextOut(hdc,10,510,appMesg,strlen(appMesg));

   gwidth=maxleft-minleft;
   scrstep=(int)(appStatus.maxiter/gwidth);
   
   if (appStatus.trnError)  {
   MoveToEx(hdc,minleft,205,NULL);
   for ( c=0; c<gwidth; c++ )
   {
       cx=c*scrstep;
       if (cx>appStatus.iter)  break;
       cy=200-appStatus.trnError[cx];
       LineTo(hdc,minleft+c,cy);
       MoveToEx(hdc,minleft+c,cy,NULL);
   }
   sprintf(strBuffer,"Trn.Error: %8.6f",appStatus.trnError[appStatus.iter-1]/100.0);
   TextOut(hdc,550,250,strBuffer,strlen(strBuffer));
   }

   if (appStatus.recptrn)  {
   MoveToEx(hdc,minleft,330,NULL);
   for ( c=0; c<gwidth; c++ )
   {
       cx=c*scrstep;
       if (cx>appStatus.iter)  break;
       cy=330-appStatus.recptrn[cx];
       LineTo(hdc,minleft+c,cy);
       MoveToEx(hdc,minleft+c,cy,NULL);
   }
   sprintf(strBuffer,"Epoch: %d of %d",appStatus.iter,appStatus.maxiter);
   TextOut(hdc,550,235,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Recognized: %d of %d",appStatus.howmany,appStatus.trnSize);
   TextOut(hdc,550,265,strBuffer,strlen(strBuffer));
   }
   
   if (appStatus.valPtg)  {
   MoveToEx(hdc,minleft,460,NULL);
   for ( c=0; c<gwidth; c++ )
   {
       cx=c*scrstep;
       if (cx>appStatus.iter)  break;
       cy=460-appStatus.valPtg[cx];
       LineTo(hdc,minleft+c,cy);
       MoveToEx(hdc,minleft+c,cy,NULL);
   }
   sprintf(strBuffer,"Validation: %6.2f%%",(float)appStatus.val_ptg*100.0);
   TextOut(hdc,550,280,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Test set: %d of %d  (%6.2f%%)",appStatus.tstptrn,appStatus.tstSize,appStatus.tstptrn*100.0/appStatus.tstSize);
   TextOut(hdc,550,300,strBuffer,strlen(strBuffer));
   if (appStatus.isOutput2)
   {
       pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0xAF,0xCF,0xAF)));
       brush=SelectObject(hdc,CreateSolidBrush(RGB(0x00,0x00,0x00)));
       Rectangle(hdc,570,380,700,480);
       Rectangle(hdc,600,350,700,480);
       MoveToEx(hdc,600,380,NULL);  LineTo(hdc,700,380);
       MoveToEx(hdc,600,430,NULL);  LineTo(hdc,700,430);
       MoveToEx(hdc,650,380,NULL);  LineTo(hdc,650,480);

       sprintf(strBuffer,"c0");
       TextOut(hdc,615,360,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"c1");
       TextOut(hdc,665,360,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"t0");
       TextOut(hdc,580,400,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"t1");
       TextOut(hdc,580,450,strBuffer,strlen(strBuffer));

       sprintf(strBuffer,"%d",appStatus.tstres[0]);
       TextOut(hdc,615,400,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"%d",appStatus.tstres[1]);
       TextOut(hdc,665,400,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"%d",appStatus.tstres[2]);
       TextOut(hdc,615,450,strBuffer,strlen(strBuffer));
       sprintf(strBuffer,"%d",appStatus.tstres[3]);
       TextOut(hdc,665,450,strBuffer,strlen(strBuffer));

       sprintf(strBuffer,"overall=%6.2f%%",(appStatus.tstres[0]+appStatus.tstres[3])*100.0/appStatus.tstSize);
       TextOut(hdc,615,500,strBuffer,strlen(strBuffer));
   }
   }

   }

   // finish drawing
   DeleteObject(pen);
   DeleteObject(brush);

   EndPaint( hWnd, &ps );
}

int GetUpdateRate( void )
{
    FILE    *iniFile;
    int     scrval;

    if ((iniFile=fopen(INIFILENAME,"r"))==NULL)
    {
        MessageBox(NULL,"Unable to open INI file",appName,MB_ICONERROR);
        return(UPDATE_EVERY);
    }
    else
    {
        fscanf(iniFile,"%d",&scrval);
        if ((scrval<1)||(scrval>UPDATE_EVERY))
        {
            MessageBox(NULL,"Invalid update interval, using default",appName,MB_ICONEXCLAMATION);
            return(UPDATE_EVERY);
        }
        fclose(iniFile);
    }

    return(scrval);
}

void ResetStatus( TStatusInfo* status, BOOL clearstats )
{
    int     c;


    if (status)
    {
        if (status->inpfile)  fclose(status->inpfile);
        if (status->logfile)  fclose(status->logfile);

        status->isInpFile=FALSE;
        status->isLogFile=FALSE;

        if (clearstats)
        {
            status->isNetwork=FALSE;

            strcpy(status->trnFileName,"");
            strcpy(status->valFileName,"");
            strcpy(status->tstFileName,"");
            strcpy(status->LogFileName,"");
            strcpy(status->InpFileName,"");

            status->ivecSize=0;  status->ovecSize=0;  status->nlay=0;
            for ( c=0; c<MAX_LAYERS; c++ )  status->lay[c]=0;

            status->maxiter=0;  status->iter=0;  status->nlay=0;
            status->trnSize=0;  status->valSize=0;  status->tstSize=0;
            if (status->recptrn)  { free(status->recptrn); status->recptrn=NULL; }
            if (status->trnError)  { free(status->trnError); status->trnError=NULL; }
            if (status->valPtg)  { free(status->valPtg); status->valPtg=NULL; }
            status->howmany=0;  status->val_ptg=0;
            
            status->tstptrn=0;
            status->isOutput2=FALSE;
        }
    }
}

BOOL isReady( TStatusInfo* status )
{
    if (!(status->isInpFile))
    {
        MessageBox(NULL, "No configuration file selected!", appName, MB_ICONERROR);
        return(FALSE);
    }

    return(TRUE);
}

void LogMessage( TStatusInfo *status, char *msg )
{
    if (status)
        if (status->logfile)
        { fprintf(status->logfile,msg); fprintf(status->logfile,"\n"); }
}

/*void ShowMessage( HWND hWnd, char *strBuffer )
{
    PAINTSTRUCT ps;
    HDC hdc;
    //char    strBuffer[256];

    
    hdc = BeginPaint( hWnd, &ps );
    
    TextOut(hdc,50,50,strBuffer,strlen(strBuffer));
    LogMessage(&appStatus,strBuffer);
    //DrawClientWindow(hWnd);
 
    EndPaint( hWnd, &ps );
}*/

void DrawMessage( HDC hdc, char *strBuffer )
{
    static RECT  rect;
    static HWND  hWnd;


    strcpy(appMesg,strBuffer);

    hWnd=WindowFromDC(hdc);
    //rect.top=510; rect.left=10; rect.bottom=600; rect.right=800;
    GetClientRect(hWnd, &rect); 
    InvalidateRect(hWnd,&rect,TRUE);
    UpdateWindow(hWnd);

    LogMessage(&appStatus,strBuffer);
}

/*void DrawClientMessages( HWND hWnd )
{
    static HDC  hdc;
    static PAINTSTRUCT ps;
    static RECT  rect;
    static char strBuffer[]="this is a test";

    hdc = BeginPaint( hWnd, &ps );
    rect.top=40; rect.left=0; rect.bottom=80; rect.right=100;
    InvalidateRect(hWnd,&rect,TRUE);
    UpdateWindow(hWnd);
    //TextOut(hdc,50,50,strBuffer,strlen(strBuffer));
    EndPaint( hWnd, &ps );
}*/

    
//**********************************************************


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
	

//#define		VERSION_TAG1	"Back-Propagation ANN Trainer, v4.12 - Universal version"
//#define		VERSION_TAG2	"Build: R000619-BSA-Win32-PPro, Harris Georgiou (c) 2000."

#define     GOOD_VAL_PTG        ((TValue)95.0)
#define     MAX_ITERATIONS	    ((TCount)30000)
#define     LIM0		        ((TValue)0.3)
#define     LIM1		        ((TValue)0.7)

#define		DEFSAVNAME		    "bpm_net.txt"
#define     AUTOSAVE_EVERY	    200


#define     round(x)            floor(x+0.5)

#endif   /* __BPM_CONST_H__  */


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

#ifndef     __BPM_IMP2_H__
#define     __BPM_IMP2_H__

//#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
//#include "bpm_cnst.h"

//-----  General purpose utility routines  -----

//... generic (fatal) error reporting ...
/*void __inline fatal( const char *mes )
{
    printf("%s", mes);
    exit(1);
}*/

void fatal( HDC hdc, char *mes )
{
    DrawMessage(hdc,mes);
    MessageBox(NULL, mes, appName, MB_ICONERROR);
    exit(1);
}


//-----  Memory buffers  -----

//... initialize buffers ...
int initialize_buffers( HDC hdc, Tmembuf *trn_buf, Tmembuf *val_buf, Tmembuf *tst_buf )
{
	TCount   i, j;
	int		 res=0;

	
	DrawMessage(hdc,"Initializing pattern set buffers...");

    //... allocate training set in/out buffers ...
	trn_buf->inpvec=(float**)calloc(trn_buf->nvec,sizeof(float*));
	if (trn_buf->inpvec)
	{
		for (i=0; i<trn_buf->nvec; i++ )
			trn_buf->inpvec[i]=(float*)calloc(trn_buf->ivecsz,sizeof(float));
	}
	if (i!=trn_buf->nvec)  fatal(hdc,"Error: trn_buf(i) not allocated"); // return(-1);

	trn_buf->outvec=(float**)calloc(trn_buf->nvec,sizeof(float*));
	if (trn_buf->outvec)
	{
		for (i=0; i<trn_buf->nvec; i++ )
			trn_buf->outvec[i]=(float*)calloc(trn_buf->ovecsz,sizeof(float));
	}
	if (i!=trn_buf->nvec)  fatal(hdc,"Error: trn_buf(o) not allocated"); // return(-1);

    //... allocate validation set in/out buffers ...
	val_buf->inpvec=(float**)calloc(val_buf->nvec,sizeof(float*));
	if (val_buf->inpvec)
	{
		for (i=0; i<val_buf->nvec; i++ )
			val_buf->inpvec[i]=(float*)calloc(val_buf->ivecsz,sizeof(float));
	}
	if (i!=val_buf->nvec)  fatal(hdc,"Error: val_buf(i) not allocated"); // return(-1);

	val_buf->outvec=(float**)calloc(val_buf->nvec,sizeof(float*));
	if (val_buf->outvec)
	{
		for (i=0; i<val_buf->nvec; i++ )
			val_buf->outvec[i]=(float*)calloc(val_buf->ovecsz,sizeof(float));
	}
	if (i!=val_buf->nvec)  fatal(hdc,"Error: val_buf(o) not allocated"); // return(-1);

    //... allocate training set in/out buffers ...
	tst_buf->inpvec=(float**)calloc(tst_buf->nvec,sizeof(float*));
	if (tst_buf->inpvec)
	{
		for (i=0; i<tst_buf->nvec; i++ )
			tst_buf->inpvec[i]=(float*)calloc(tst_buf->ivecsz,sizeof(float));
	}
	if (i!=tst_buf->nvec)  fatal(hdc,"Error: tst_buf(i) not allocated"); // return(-1);

	tst_buf->outvec=(float**)calloc(tst_buf->nvec,sizeof(float*));
	if (tst_buf->outvec)
	{
		for (i=0; i<tst_buf->nvec; i++ )
			tst_buf->outvec[i]=(float*)calloc(tst_buf->ovecsz,sizeof(float));
	}
	if (i!=tst_buf->nvec)  fatal(hdc,"Error: tst_buf(o) not allocated"); // return(-1);


	DrawMessage(hdc,"Reading pattern sets into buffers...");
	
    //... read training set into buffers
    for ( i=0; i<trn_buf->nvec; i++ )
	{
		for( j=0; j<trn_buf->ivecsz; j++ )
		{
			fscanf(trn_buf->fh,"%f",&(trn_buf->inpvec[i][j]));
			//printf("%7.3f",trn_buf->inpvec[i][j]);
		}
		//printf("\n");
    
		for( j=0; j<trn_buf->ovecsz; j++ )
		{
			fscanf(trn_buf->fh,"%f",&(trn_buf->outvec[i][j]));
			//printf("%7.3f",trn_buf->outvec[i][j]);
		}
		//printf("\n");
	}

    //... read validation set into buffers
    for ( i=0; i<val_buf->nvec; i++ )
	{
		for( j=0; j<val_buf->ivecsz; j++ )
			fscanf(val_buf->fh,"%f",&(val_buf->inpvec[i][j]));
    
		for( j=0; j<val_buf->ovecsz; j++ )
			fscanf(val_buf->fh,"%f",&(val_buf->outvec[i][j]));
	}

    //... read test set into buffers
    for ( i=0; i<tst_buf->nvec; i++ )
	{
		for( j=0; j<tst_buf->ivecsz; j++ )
			fscanf(tst_buf->fh,"%f",&(tst_buf->inpvec[i][j]));
    
		for( j=0; j<tst_buf->ovecsz; j++ )
			fscanf(tst_buf->fh,"%f",&(tst_buf->outvec[i][j]));
	}

    trn_buf->pos=0;
    val_buf->pos=0;
    tst_buf->pos=0;
 
    DrawMessage(hdc,"done!");

    return(0);
}

//... finallize buffers ...
void close_buffers( Tmembuf *trn_buf, Tmembuf *val_buf, Tmembuf *tst_buf )
{
	TCount   i;

	for ( i=0; i<trn_buf->nvec; i++ )
	{
		free(trn_buf->inpvec[i]);
		free(trn_buf->outvec[i]);
	}
	free(trn_buf->inpvec);
	free(trn_buf->outvec);

	for ( i=0; i<val_buf->nvec; i++ )
	{
		free(val_buf->inpvec[i]);
		free(val_buf->outvec[i]);
	}
	free(val_buf->inpvec);
	free(val_buf->outvec);

	for ( i=0; i<tst_buf->nvec; i++ )
	{
		free(tst_buf->inpvec[i]);
		free(tst_buf->outvec[i]);
	}
	free(tst_buf->inpvec);
	free(tst_buf->outvec);
}


//-----  File I/O functions  -----

//... get patterns files (names) ...
void openfiles( char *patfname, TCount *npat, char *valfname, TCount *nval, char *testfname, TCount *ntest )
{
    //printf("Enter training patterns file name: ");
   	fscanf(appStatus.inpfile,"%s", patfname);
    //printf("Enter #training patterns: ");
    fscanf(appStatus.inpfile,"%d", npat);  
    strcpy(appStatus.trnFileName,patfname);
    appStatus.trnSize=*npat;

    //printf("Enter validation patterns file name: ");
   	fscanf(appStatus.inpfile,"%s", valfname);
    //printf("Enter #validation patterns: ");
    fscanf(appStatus.inpfile,"%d", nval);  
    strcpy(appStatus.valFileName,valfname);
    appStatus.valSize=*nval;

    //printf("Enter testing patterns file name: ");
   	fscanf(appStatus.inpfile,"%s", testfname);
    //printf("Enter #testing paterns: ");
    fscanf(appStatus.inpfile,"%d", ntest);  
    strcpy(appStatus.tstFileName,testfname);
    appStatus.tstSize=*ntest;
}

//... initialize patterns files (open) ...
int initfiles( HDC hdc, char *patfname, FILE **pat, Tmembuf *trn_buf, TCount npat,
                char *valfname, FILE **val, Tmembuf *val_buf, TCount nval,
                char *testfname, FILE **test, Tmembuf *tst_buf, TCount ntest )
{
    char    msg[256];

    if ((*pat = fopen(patfname,"r"))==NULL)
    {
        sprintf(msg,"Invalid training file ('%s')\n",patfname);
        fatal(hdc,msg);
        //return(1);
    }

    if ((*val = fopen(valfname,"r"))==NULL)
    {
        sprintf(msg,"Invalid validation file ('%s')\n",valfname);
        fatal(hdc,msg);
        //return(2);
    }

    if ((*test = fopen(testfname,"r"))==NULL)
    {
        sprintf(msg,"Invalid testing file ('%s')\n",testfname);
        fatal(hdc,msg);
        //return(3);
    }

    trn_buf->fh=*pat;  trn_buf->nvec=npat;
    val_buf->fh=*val;  val_buf->nvec=nval;
    tst_buf->fh=*test; tst_buf->nvec=ntest;

    return(0);
}

//... close patterns files ...
void __inline closefiles( FILE **pat, FILE **val, FILE **test )
{
   fclose(*pat);
   fclose(*val);
   fclose(*test);
}

//... read patterns from file (disabled, use buffered I/O instead) ...
/*void __inline readpat( FILE *patfile, float *vec, int vecsize, float *dis, int dis_size)
{
    TCount i;

    for( i=0; i<vecsize; i++ )
	    fscanf(patfile, "%f", &vec[i] );
    for( i=0; i<dis_size; i++ )
	    fscanf(patfile, "%f", &dis[i] );

}*/

//... read patterns from buffer ...
void __inline readpat_buffered( Tmembuf *patbuff, float *vec, int vecsize, float *dis, int dis_size)
{
    TCount i;

    for( i=0; i<vecsize; i++ )
		vec[i]=patbuff->inpvec[patbuff->pos][i];
    for( i=0; i<dis_size; i++ )
		dis[i]=patbuff->outvec[patbuff->pos][i];

	patbuff->pos++;

}

//... rewind patterns buffer ...
void __inline rewind_buffered( Tmembuf *patbuff )
{
	patbuff->pos=0;
}


//-----  Network routines  -----

//... initialize neurons ...
int __inline init_neuron( TNeuron *neu, TCount nweights, TWeight *weights, TWeight theta)
{
    TCount      count=0;
    
    if ((neu->w = calloc(nweights,sizeof(TWeight)))==NULL || 
	(neu->wprev = calloc(nweights,sizeof(TWeight)))==NULL )
          return(1);
    else
    {
    	neu->wnum = nweights;
        for ( count=0; count<nweights; count++ )
        {
        	neu->w[count] =  weights[count];
        	neu->wprev[count] = weights[nweights - 1 - count];
        }
        neu->h  = theta;
        neu->eval = (TWeight)0;
        return(0);
    }
}

//... initialize layer ...                                                                                
int __inline init_layer( TLayer *lay, TCount nneurons )
{
    if ((lay->neu = calloc(nneurons,sizeof(TNeuron)))==NULL)
    	return(1);              /* no initialization for array objects */
    else
    {
    	lay->nnum = nneurons;
        return(0);
    }
}
                                        
//... initialize network ...
int __inline init_net( TNet *net, TCount nlayers )
{
    if ((net->lay = calloc(nlayers,sizeof(TLayer)))==NULL)
    	return(1);              /* no initialization for array objects */
    else
    {
    	net->lnum = nlayers;
        return(0);
    }
}

//... read parameters & create network ...
void create_net( HDC hdc, TNet *BP_net, TNet *aux_net, TWeight *coeff, TWeight *mcoeff,
                TCount *maxiters, TValue *goodvalptg, Tmembuf *trn_buf, Tmembuf *val_buf, Tmembuf *tst_buf )
{
    TCount      nlayers=0, nlay=0;
    TCount      nneu=0;
    TCount      nweights=0, nwei=0;
    TCount      /*neurons[MAXNEURONNUM],*/ *neurons;
    TWeight     /*weight[MAXWEIGHTNUM],*/ *weight;
    int         res=0;
    char        msg[256];
                    
    //printf("Give number of neuron layers: ");
    fscanf(appStatus.inpfile,"%d",&nlayers);
    res = init_net( BP_net, nlayers )
		+  init_net( aux_net, nlayers );
    if (res==0)
    {
    appStatus.nlay=nlayers;
	DrawMessage(hdc,"Memory for net allocated correctly!\n");
	
	//printf("Give dimension of input vector: ");
    fscanf(appStatus.inpfile,"%d", &nweights);

	trn_buf->ivecsz=nweights;
	val_buf->ivecsz=nweights;
	tst_buf->ivecsz=nweights;
    appStatus.ivecSize=nweights;

    if ((neurons=(TCount*)calloc(nlayers,sizeof(TCount)))==NULL)
		fatal(hdc,"Error: cannot allocate temp memory for neurons");

	for ( nlay=0; nlay<nlayers; nlay++ )
	{
	    //printf("\tGive number of neurons in layer #%d: ",nlay+1);
	    fscanf(appStatus.inpfile,"%d",&neurons[nlay]);
	    res = init_layer( &(BP_net->lay[nlay]), neurons[nlay] ) 
	          + init_layer( &(aux_net->lay[nlay]), neurons[nlay] );
	    if (res==0)
	    {
        appStatus.lay[nlay]=neurons[nlay];
		sprintf(msg,"Memory allocated for layer #%d.", nlay+1);
        DrawMessage(hdc,msg);

		for( nneu=0; nneu<neurons[nlay]; nneu++ )
	    	{	
			if ((weight=(TWeight*)calloc(nweights+1,sizeof(TWeight)))==NULL)
				fatal(hdc,"Error: cannot allocate temp memory for weights");

		    for( nwei=0; nwei<nweights+1; nwei++ )
		 	    weight[nwei]= (TWeight)(rand() / (TWeight)RAND_MAX - 0.5);
		    res = init_neuron( &(BP_net->lay[nlay].neu[nneu]), nweights, weight, weight[nweights])
		          + init_neuron( &(aux_net->lay[nlay].neu[nneu]), nweights, weight, weight[nweights]);
		    if(res)
		    {
			    sprintf(msg,"ERROR: Cannot allocate memory for neuron #%d in layer #%d\n", nneu, nlay);
                fatal(hdc,msg);
		    }

			free(weight);
	        }

		nweights = neurons[nlay];   
	    }
	    else
	    {	
		    sprintf(msg,"ERROR: Cannot allocate memory for layer %d\n",nlay);
		    fatal(hdc,msg);
	    }
	}
    
    appStatus.nlay=nlayers;
	free(neurons);

	trn_buf->ovecsz=nweights;
	val_buf->ovecsz=nweights;
	tst_buf->ovecsz=nweights;
    appStatus.ovecSize=nweights;

	//printf("\nEnter the learning coefficient (0..1): ");
	fscanf(appStatus.inpfile,"%f", coeff);
    *coeff=(TValue)fabs(*coeff);
    appStatus.lcoeff=*coeff;

	//printf("\nEnter the momentum coefficient (0..1): ");
	fscanf(appStatus.inpfile,"%f", mcoeff);
    *mcoeff=(TValue)fabs(*mcoeff);
    appStatus.mcoeff=*mcoeff;

	//printf("\nEnter the maximum number of epochs (0..%d): ",MAX_ITERATIONS);
	fscanf(appStatus.inpfile,"%d", maxiters);
    *maxiters=abs(*maxiters);
    if ((*maxiters<1)||(*maxiters>MAX_ITERATIONS))  *maxiters=MAX_ITERATIONS;

    appStatus.maxiter=(*maxiters);
    appStatus.recptrn=(int*)calloc(appStatus.maxiter,sizeof(int));
    appStatus.trnError=(int*)calloc(appStatus.maxiter,sizeof(int));
    appStatus.valPtg=(int*)calloc(appStatus.maxiter,sizeof(int));
    if ((!appStatus.recptrn)||(!appStatus.trnError)||(!appStatus.valPtg))
        fatal(hdc,"Not enough memory for graph data.");


   	//printf("\nEnter the validation %%success limit (%5.1f%%..%5.1f%%): ",GOOD_VAL_PTG,100.0);
	fscanf(appStatus.inpfile,"%f", goodvalptg);
    *goodvalptg=(TValue)fabs(*goodvalptg/100.0);
    if (*goodvalptg<GOOD_VAL_PTG/100.0)  *goodvalptg=(TValue)(GOOD_VAL_PTG/100.0);
    if (*goodvalptg>1.0)  *goodvalptg=(TValue)1.0;
    appStatus.goodvalptg=(*goodvalptg)*100;
    
    }
    else
       fatal(hdc,"ERROR: Cannot allocate memory for layers\n");

}

//... finallize neuron ...
void __inline done_neuron( TNeuron *neu )
{
    free(neu->w);
    free(neu->wprev);
}

//... finallize layer ...
void __inline done_layer( TLayer *lay )
{
    TCount      nneurons;
    for ( nneurons=0; nneurons<(lay->nnum); nneurons++ )
        done_neuron( &(lay->neu[nneurons]) );
    free(lay->neu);
}

//... finallize network (internals) ...
void __inline ddone_net( TNet *net )
{
    TCount      nlayers;
    for ( nlayers=0; nlayers<(net->lnum); nlayers++ )
        done_layer( &((net->lay)[nlayers]) );
    free(net->lay);
}

//... finallize network ...
void __inline done_net(TNet *bp, TNet *aux)
{
    ddone_net(bp);
    ddone_net(aux);
}

//... print all network variables ...                    
/*void print_net(TNet *net)
{
    TCount nlay,  nneu, i;
    char    msg[256];

    for(nlay=0; nlay < (net->lnum); nlay++)
    {
    	for(nneu=0; nneu < (net->lay[nlay].nnum); nneu++)
	    {
	        for(i=0; i < (net->lay[nlay].neu[nneu].wnum); i++)
	        {
		        sprintf(msg,"Layer #%d ,", nlay+1);
                DrawMessage(hdc,msg);
		        sprintf(msg," neuron #%d ,", nneu+1);
                DrawMessage(hdc,msg);
		        sprintf(msg," weight #%d = %f", i+1, net->lay[nlay].neu[nneu].w[i] );
                DrawMessage(hdc,msg);
		        printf(" previous weight #%d = %f\n", i+1, net->lay[nlay].neu[nneu].wprev[i] );
	        }
	        printf("Layer #%d ,", nlay+1);
	        printf(" neuron #%d ,", nneu+1);
	        printf(" threshold = %f\n", net->lay[nlay].neu[nneu].h);
	    }
	    printf("--------------------------------------------------------------------------------\n");
    }
   
    for(nneu=0; nneu < (net->lay[net->lnum - 1].nnum); nneu++)
        printf("out#%d = %f ", nneu+1, net->lay[net->lnum - 1].neu[nneu].eval);
    printf("\n================================================================================\n");
}
*/

//... save network topology & weights into file ...
long quicksave_net( HDC hdc, TNet *opt_net, char *fname )
{
	FILE   *ax_fh;
	int    res=0;
    TCount nlay,  nneu, nwei;
    char    msg[256];


	sprintf(msg,"Saving current network in file (%s)...",fname);
    DrawMessage(hdc,msg);

	ax_fh=fopen(fname,"w+");
	if (!ax_fh)
	{
		sprintf(msg,"Error: unable to open file '%s'... ",fname);
        DrawMessage(hdc,msg);
	}

	/*... write net topology in file header ...*/
	// # of layers
	//res += fwrite(&(opt_net->lnum),sizeof(TCount),1,ax_fh);
	res += fprintf(ax_fh,"%d\n",opt_net->lnum);

	// # of input vector dimension
	//res += fwrite(&(opt_net->lay[0].neu[0].wnum),sizeof(TCount),1,ax_fh);
	res += fprintf(ax_fh,"%d\n",opt_net->lay[0].neu[0].wnum);

	// # of neuron weight vector dimensions
	for(nlay=0; nlay < (opt_net->lnum); nlay++)
		//res += fwrite(&(opt_net->lay[nlay].nnum),sizeof(TCount),1,ax_fh);
		res += fprintf(ax_fh,"%d\n",opt_net->lay[nlay].nnum);

	//... write all network weights ...
	for(nlay=0; nlay < (opt_net->lnum); nlay++)
	{
    	for(nneu=0; nneu < (opt_net->lay[nlay].nnum); nneu++)
		{
			//res += fwrite(&(opt_net->lay[nlay].neu[nneu].w),sizeof(TWeight),opt_net->lay[nlay].neu[nneu].wnum,ax_fh);
			for ( nwei=0; nwei<opt_net->lay[nlay].neu[nneu].wnum; nwei++ )
				res += fprintf(ax_fh,"%f   ",opt_net->lay[nlay].neu[nneu].w[nwei]);
			//res += fwrite(&(opt_net->lay[nlay].neu[nneu].h),sizeof(TWeight),1,ax_fh);
			res += fprintf(ax_fh,"\n%f\n",opt_net->lay[nlay].neu[nneu].h);
		}
	}

	fclose(ax_fh);

	return(res);
}

//... copy (deep) one network to another ...
void copy_net(TNet *s, TNet *t)
{
    TCount clay, cneu, cwei;
    
    for(clay = 0; clay < s->lnum; clay++)
    	for(cneu = 0; cneu < s->lay[clay].nnum; cneu++)
    	{   
    	   for(cwei = 0; cwei < s->lay[clay].neu[cneu].wnum; cwei++)
    	   {
    	     t->lay[clay].neu[cneu].w[cwei]	= s->lay[clay].neu[cneu].w[cwei];
    	     t->lay[clay].neu[cneu].wprev[cwei]	= s->lay[clay].neu[cneu].wprev[cwei];
    	   }
    	   t->lay[clay].neu[cneu].h		= s->lay[clay].neu[cneu].h;
    	}
}

//... transfer function (sigmoid or tanh) ...
TValue __inline sigmoid( TValue val )
{
    return (TValue)(1.0 / ( 1.0 + exp(-val) ));
}

//... calculate network output ...
void calc_output( TNet *bp, const float *vec )
{

    TCount 	clay, cneu, cwei;
    TValue	newval;

    for ( cneu=0; cneu < bp->lay[0].nnum; cneu++)	/* First layer */
	{
	    newval = (TValue)0;
	    for( cwei=0; cwei < bp->lay[0].neu[cneu].wnum; cwei++ )
		newval += bp->lay[0].neu[cneu].w[cwei] * vec[cwei];
	    newval += bp->lay[0].neu[cneu].h;
	    bp->lay[0].neu[cneu].eval = sigmoid( newval );
	}

    for( clay=1; clay < bp->lnum; clay++)		/* Rest of layers */
	    for(cneu=0; cneu < bp->lay[clay].nnum; cneu++)
	    {
		    newval = (TValue)0;
		    for( cwei=0; cwei < bp->lay[clay].neu[cneu].wnum; cwei++)
		        newval += bp->lay[clay].neu[cneu].w[cwei] * bp->lay[clay-1].neu[cwei].eval;
		    newval += bp->lay[clay].neu[cneu].h;

            // absolute output
		    bp->lay[clay].neu[cneu].eval = sigmoid( newval );
	    }
}

//... update network weights ...
void update_weights( HDC hdc, TNet *bp, const float *vec, const float *dis, const TWeight coeff, const TWeight mcoeff )
{

    TCount	clay, cneu,	cwei, upneu;
    TWeight **error, temp;
    TValue val;

    if ((error=(TWeight**)calloc(bp->lnum,sizeof(TWeight*)))==NULL)
        fatal(hdc,"Abort: weight pool allocated null.\n");

    for (clay = 0; clay < bp->lnum; clay++)
    {
	    if( (error[clay] = (TWeight*)calloc( bp->lay[clay].nnum, sizeof(TWeight)) ) == NULL )
	        fatal(hdc,"ERROR: not enough memory. Program aborted.\n");
    }

    for(cneu = 0; cneu < bp->lay[bp->lnum - 1].nnum; cneu++)
    {
	    val = bp->lay[bp->lnum - 1].neu[cneu].eval;
	    error[bp->lnum - 1][cneu] = (TWeight)((dis[cneu] - val)*val*(1.0 - val));
    }

    for(clay = (bp->lnum) - 2; clay >= 0; clay--)
    {
	    for(cneu=0; cneu < bp->lay[clay].nnum; cneu++)
	    {
	        error[clay][cneu] = (TWeight)0;
	        for(upneu=0; upneu < bp->lay[clay + 1].nnum; upneu++)
		        error[clay][cneu] += error[clay+1][upneu] * bp->lay[clay + 1].neu[upneu].w[cneu];
	        val = bp->lay[clay].neu[cneu].eval;
	        error[clay][cneu] *=  (TWeight)(val * (1.0 - val));
	    }
    }

    for( clay = bp->lnum - 1; clay > 0; clay--)
    {
	    for(cneu = 0; cneu < bp->lay[clay].nnum; cneu++)
	    {
		    for(cwei = 0; cwei < bp->lay[clay].neu[cneu].wnum; cwei++)
		    {
		        temp = bp->lay[clay].neu[cneu].w[cwei];
		        bp->lay[clay].neu[cneu].w[cwei] += coeff * error[clay][cneu] * bp->lay[clay-1].neu[cwei].eval + mcoeff * (bp->lay[clay].neu[cneu].w[cwei] - bp->lay[clay].neu[cneu].wprev[cwei]);
		        bp->lay[clay].neu[cneu].wprev[cwei] = temp;
		    }
    	    bp->lay[clay].neu[cneu].h += coeff * error[clay][cneu];
	   }
     }

    for(cneu = 0; cneu < bp->lay[0].nnum; cneu++)
    {
	    for(cwei = 0; cwei < bp->lay[0].neu[cneu].wnum; cwei++)
		{
    	    temp = bp->lay[0].neu[cneu].w[cwei];
		    bp->lay[0].neu[cneu].w[cwei] += coeff * error[0][cneu] * vec[cwei] + mcoeff * (bp->lay[0].neu[cneu].w[cwei] - bp->lay[0].neu[cneu].wprev[cwei]);
		    bp->lay[0].neu[cneu].wprev[cwei] = temp;
		}
		bp->lay[0].neu[cneu].h += coeff * error[0][cneu];
	}

    for(clay = 0; clay < bp->lnum; clay++)
    {
        if(error[clay])  free(error[clay]);
    }
    free(error);
}

//... print (current) network output ...
/*void print_out(TNet *bp, float *dis)
{
    TCount cneu;
    float val;

    for(cneu=0; cneu < bp->lay[bp->lnum - 1].nnum; cneu++)
    {
	    if( bp->lay[bp->lnum - 1].neu[cneu].eval <= LIM0 )
	        val = 0.0;
	    else
        {
	        if( bp->lay[bp->lnum - 1].neu[cneu].eval >= LIM1 )
		        val = 1.0;
	        else
		        val = (float)(1.0 - dis[cneu]);
        }
		
	    printf("\nnet_out = %f dis_out = %f", val, dis[cneu]);
    }
    printf("\n");
}*/
   
//... compare network output with desired ...
int recognised(TNet *bp, float *dis)
{

    TCount	cneu;
    float val;

	
    for(cneu=0; cneu < bp->lay[bp->lnum - 1].nnum; cneu++)
    {
	    if( bp->lay[bp->lnum - 1].neu[cneu].eval <= LIM0 )
	        val = 0.0;
	    else
        {
	        if( bp->lay[bp->lnum - 1].neu[cneu].eval >= LIM1 )
		        val = 1.0;
	        else
		        val = (float)(1.0 - dis[cneu]);
        }
        
        if( val != dis[cneu] )
	        return 0;
    }

    return(1);
}

//... calculate network output error ...
TValue __inline calcerror(TNet *bp, float *dis, int ds)
{
   TCount cneu;
   TValue err=0.0;
   
   for(cneu=0; cneu<ds; cneu++)
       err = err + (TValue)(pow(bp->lay[bp->lnum - 1].neu[cneu].eval - dis[cneu], 2));
   
   return(err);
   
}

//... check validation set ...
int check_validation( HDC hdc, Tmembuf *val_buf, int valsize, TValue *meanerr, TNet *bp, TNet *aux)
{
    TCount identified = 0, valcount;
    static TCount best_score = 0;
    static TValue min_err = FLT_MAX;
    TValue err;
    int vecsize, dis_size;
    float *vec, *dis;

    vecsize = bp->lay[0].neu[0].wnum;   					
    dis_size = bp->lay[bp->lnum - 1].nnum;
   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis))
        fatal(hdc,"ERROR: Cannot allocate memory. Program aborted.\n");

    err = 0.0;
    for(valcount = 0; valcount < valsize; valcount++)
    {
		readpat_buffered(val_buf,vec,vecsize,dis,dis_size);
    	calc_output(bp,vec);
    	err += calcerror(bp,dis,dis_size);
        identified += recognised(bp,dis);
    }
    
	rewind_buffered(val_buf);
    err /= (TValue)valsize;
    //printf("\nIn val-set identified = %d mean-err = %f, ", identified, err);
    *meanerr = err;

    if((identified >= best_score) && (err <= min_err) )
    {
    	copy_net(bp, aux);
    	best_score = identified;
    	min_err = err;
    }
    
    free(vec);
    free(dis);
    return identified;
    
}

//... check test set ...
TCount check_testset( HDC hdc, Tmembuf *tst_buf, int testsize, TValue *meanerr, TNet *bp )
{
    TCount identified = 0, valcount;
    static TCount best_score = 0;
    static TValue min_err = FLT_MAX;
    TValue err;
    int vecsize, dis_size, recognized;
    float *vec, *dis;
    char    msg[256];

    vecsize = bp->lay[0].neu[0].wnum;  					
    dis_size = bp->lay[bp->lnum - 1].nnum;
   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis) )
               fatal(hdc,"ERROR: Cannot allocate memory. Program aborted.\n");

   	rewind_buffered(tst_buf);

    err = 0.0;
    for(valcount = 0; valcount < testsize; valcount++)
    {
		readpat_buffered(tst_buf, vec, vecsize, dis, dis_size);
    	calc_output(bp,vec);
    	err += calcerror(bp, dis, dis_size);
        recognized = recognised(bp, dis);
        identified += recognized;

        sprintf(msg,"#test.pattern=%d , tst.error=%8.3f , recognized=%s",valcount,err/(valcount+1),((recognized)?"YES":"NO"));
        DrawMessage(hdc,msg);
    }
    
    err /= (TValue)testsize;
    //printf("\nIn val-set identified = %d mean-err = %f, ", identified, err);
    *meanerr = err;

    sprintf(msg,"#test.patterns=%d , tst.error=%8.3f , recognized=%d",testsize,*meanerr,identified);
    DrawMessage(hdc,msg);

    free(vec);
    free(dis);
    return(identified);
}

//... check test set, create truth table for 2-categories classification ...
TCount check_testset_c2( HDC hdc, Tmembuf *tst_buf, int testsize, TValue *meanerr, TCount *result, TNet *bp )
{
    TCount  identified=0, valcount;
    static  TCount best_score = 0;
    static  TValue min_err = FLT_MAX;
    TValue  err;
    int     vecsize, dis_size, res;
    float   *vec, *dis;

    
    vecsize = bp->lay[0].neu[0].wnum;
    dis_size = bp->lay[bp->lnum - 1].nnum;
   
    if (dis_size==2)
        for ( valcount=0; valcount<4; valcount++ )  result[valcount]=0;

    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis) )
               fatal(hdc,"ERROR: Cannot allocate memory. Program aborted.\n");

	rewind_buffered(tst_buf);

    err = 0.0;
    for(valcount = 0; valcount < testsize; valcount++)
    {
		readpat_buffered(tst_buf, vec, vecsize, dis, dis_size);
    	calc_output(bp,vec);
    	err += calcerror(bp, dis, dis_size);
        res = recognised(bp, dis);
        identified += res;

        if (dis_size==2)
        {
            if (res)    // if classified correctly,
            {
                if ((int)round(dis[0])==1)  result[0]++;    // then update counter at [0,0]
                else  result[3]++;                          // or counter at [1,1]
            }
            else        // else if false classification,
            {
                if ((int)round(dis[0])==1)  result[1]++;    // then update counter at [0,1]
                else  result[2]++;                          // or counter at [1,0]
            }
        }
    }
    
    err /= (TValue)testsize;
    //printf("\nIn val-set identified = %d mean-err = %f, ", identified, err);
    *meanerr = err;

    free(vec);
    free(dis);
    return(identified);
}

//... check training set ...
int __inline allpaterns( Tmembuf *trn_buf, TCount *howmany, TCount *patcount, TCount npat, TCount *iter)
{
    if(*patcount == npat )      // if and of training set is reached,
    {
        *patcount = 1;              // reset current pattern counter,
	    (*iter)++;                  // increase training iterations,
	    rewind_buffered(trn_buf);   // and rewind I/O structure for set.
	    
        if(*howmany == npat)    // if all patterns were recognized correctly, 
	        return 1;               // signal for the end of training
        else                    // if not all patterns were recognized,
            *howmany = 0;           // reset counter and restart.
    }
    else                        // if current iteration continues,
	    (*patcount)++;              // just move to the next pattern.

   return(0);
}

//... network training ...
void teach_net(HDC hdc,TNet *bp, TNet *aux,
	       Tmembuf *trn_buf, TCount npat, Tmembuf *val_buf, TCount valsize, 
	       TWeight coeff, TWeight mcoeff, TCount maxiters, TValue goodvalptg )
{
    TCount vecsize, dis_size, howmany = 0, cpat=1, iterations=1;
    TValue val_percentage, meanerr;
    int learningOK = 0;
    float *vec, *dis;
    int eflag;
    char    msg[256];

    
    vecsize = bp->lay[0].neu[0].wnum; 					
    dis_size = bp->lay[bp->lnum - 1].nnum;
   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis))
   	   fatal(hdc,"ERROR: Cannot allocate memory. Program aborted.\n");

    //... main training loop ... 
    while (!learningOK)
    {   
        val_percentage=0;

        for (cpat=0; cpat<npat; cpat++)
        {
	        readpat_buffered(trn_buf,vec,vecsize,dis,dis_size);
            calc_output(bp,vec);
            update_weights(hdc,bp,vec,dis,coeff,mcoeff);

            howmany += recognised(bp,dis);
            val_percentage += (TValue)check_validation(hdc,val_buf,valsize,&meanerr,bp,aux);
        }	

        val_percentage = val_percentage / (TValue)(valsize*npat);

        
        appStatus.recptrn[appStatus.iter]=(int)(howmany*100.0/appStatus.trnSize);
        appStatus.howmany=howmany;
        appStatus.valPtg[appStatus.iter]=(int)round(val_percentage*100.0);
        appStatus.val_ptg=val_percentage;
        appStatus.trnError[appStatus.iter]=(int)round(meanerr*100.0);
        appStatus.iter++;

        if (((iterations-1) % screenupdate)==0)
        {
            sprintf(msg,"#iter=%d of %d , recogn= %d of %d, trainerr=%8.6f , validated=%6.2f%%", iterations, maxiters, howmany, npat, meanerr, val_percentage*100.0);
            DrawMessage(hdc,msg);
        }

        eflag=allpaterns(trn_buf, &howmany, &cpat, npat, &iterations);
      
        if ((eflag==1)||(val_percentage>=goodvalptg)||(iterations>maxiters)) 
            learningOK = 1;

	    /*... save best network every AUTOSAVE_EVERY iterations ...*/
	    if (!(iterations % AUTOSAVE_EVERY))  quicksave_net(hdc,aux,DEFSAVNAME);
    }
    //printf("\n EFLAG: %d - HOWMANY: %d - ITERATIONS: %d\n", eflag, howmany, iterations); 

    sprintf(msg,"#iter=%d of %d , recogn= %d of %d, trainerr=%8.6f , validated=%6.2f%%", iterations, maxiters, howmany, npat, meanerr, val_percentage*100.0);
    DrawMessage(hdc,msg);

    copy_net(aux, bp);
    free(vec);
    free(dis);

    //EndPaint( hWnd, &ps );
}

//... network testing ...
void test_net(HDC hdc,TNet *bp, Tmembuf *tst_buf, TCount ntest, TWeight lcoeff, TWeight mcoeff )
{
    TCount vecsize = bp->lay[0].neu[0].wnum, 
           dis_size = bp->lay[bp->lnum - 1].nnum, 
           correct = 0, result[4];
    float *vec, *dis, ptg_success, ptg_failure, meanerr;
    char msg[256];
    
    //printf("Inside test: vecsize = %d, dis_size = %d\n\n", vecsize, dis_size);   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis))
        fatal(hdc,"ERROR: Cannot allocate memory. Program aborted.\n");
   
    correct=check_testset(hdc,tst_buf,ntest,&meanerr,bp);
    ptg_success = (float)(100.0*correct/(TValue)ntest);
    ptg_failure = (float)(100.0-ptg_success);
    appStatus.tstptrn=correct;
/*
    printf("\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  #paterns tested        #paterns recongised         #failures            |\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  %15d        %19d         %9d            |\n", ntest, correct, ntest-correct);
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  Percentages:           %19.3f%%        %9.3f%%           |\n", ptg_success, ptg_failure); 
    printf("+--------------------------------------------------------------------------+\n");
    printf("    Learning coefficient: %5.3f     Momentum coefficient: %5.3f\n\n",lcoeff,mcoeff);
*/
    //... restart testing, cross check results with 'hits-misses' truth table ...
    if (bp->lay[bp->lnum-1].nnum == 2)
    {
        check_testset_c2(hdc,tst_buf,ntest,&meanerr,result,bp);
        appStatus.isOutput2=TRUE;
        appStatus.tstres[0]=(int)result[0];
        appStatus.tstres[1]=(int)result[1];
        appStatus.tstres[2]=(int)result[2];
        appStatus.tstres[3]=(int)result[3];
/*
        printf("\t      cN    cA  \n");
        printf("\t    +-----------+\n");
	    printf("\tpN: | %3ld | %3ld |\n",(long)result[0],(long)result[1]);
	    printf("\t    +-----------+\n");
	    printf("\tpA  | %3ld | %3ld |\n",(long)result[2],(long)result[3]);
	    printf("\t    +-----------+\n");
	    printf("\t    succ = %5.1f%%\n",((double)(result[0]+result[3]))/ntest*100);
*/
    }

    sprintf(msg,"patterns tested=%d, correct=%d (%5.1f), failure=%d (%5.1f)",ntest,correct,ptg_success,ntest-correct,ptg_failure);
    DrawMessage(hdc,msg);
    //MessageBox(NULL,"Testing finished!", appName, MB_ICONINFORMATION);

    free(vec);
    free(dis);
}

    		
#endif   /* __BPM_IMP2_H__  */


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

//#include "bpm_imp2.h"


int __cdecl console_main( HWND hWnd )
{
    TNet	BP_net, aux_net;
    TWeight	LCOEFF, MCOEFF;
    FILE	*patfile, *valfile, *testfile;
    char    patfname[30], valfname[30], testfname[30], msg[256];
    TCount	npat = 0, nval = 0, ntest = 0, maxiters;
    TValue  goodvalptg;
    Tmembuf trn_buf, val_buf, tst_buf;
    int     res;
    PAINTSTRUCT ps;
    HDC hdc;


    hdc = BeginPaint( hWnd, &ps );

    //printf("%s\n%s\n",VERSION_TAG1,VERSION_TAG2);
    //printf("--------------------------------------------------------\n\n");

    sprintf(msg,"Opening patterns files...");
    DrawMessage(hdc,msg);
    openfiles(patfname,&npat,valfname,&nval,testfname,&ntest);
    res=initfiles(hdc,patfname,&patfile,&trn_buf,npat,valfname,&valfile,&val_buf,nval,testfname,&testfile,&tst_buf,ntest);
    if (res)  fatal(hdc,"Error: unable to read input files\n");
    
    DrawMessage(hdc,"Creating network topology...");
    create_net(hdc,&BP_net,&aux_net,&LCOEFF,&MCOEFF,&maxiters,&goodvalptg,&trn_buf,&val_buf,&tst_buf);

    DrawMessage(hdc,"Initializing file buffers...");
    initialize_buffers(hdc,&trn_buf,&val_buf,&tst_buf);
    /*printf("trn_buf: nvec=%d, ivecsz=%d, ovecsz=%d",trn_buf.nvec,trn_buf.ivecsz,trn_buf.ovecsz);
    printf("val_buf: nvec=%d, ivecsz=%d, ovecsz=%d",val_buf.nvec,val_buf.ivecsz,val_buf.ovecsz);
    printf("tst_buf: nvec=%d, ivecsz=%d, ovecsz=%d",tst_buf.nvec,tst_buf.ivecsz,tst_buf.ovecsz);*/

    appStatus.isNetwork=TRUE;
    sprintf(msg,"Start learning (stop at: val%=%5.1f%% or epoch>%d) .............",goodvalptg*100.0,maxiters);
    DrawMessage(hdc,msg);
    teach_net(hdc,&BP_net,&aux_net,&trn_buf,npat,&val_buf,nval,LCOEFF,MCOEFF,maxiters,goodvalptg);
    MessageBox(NULL,"Training finished!", appName, MB_ICONINFORMATION);

    DrawMessage(hdc,"Finished training. Start testing.............");
    test_net(hdc,&BP_net,&tst_buf,ntest,LCOEFF,MCOEFF);
    MessageBox(NULL,"Testing finished!", appName, MB_ICONINFORMATION);

    DrawMessage(hdc,"Saving optimal network into file...");
	quicksave_net(hdc,&aux_net,DEFSAVNAME);
    
    DrawMessage(hdc,"Closing patterns files...");
    closefiles(&patfile,&valfile,&testfile);
    //fcloseall();
	//printf("De-allocating file buffers...\n");
    close_buffers(&trn_buf,&val_buf,&tst_buf);

    DrawMessage(hdc,"De-allocating network structure...");
    done_net(&BP_net,&aux_net);

    DrawMessage(hdc,"Training process terminated normally.");

    EndPaint( hWnd, &ps );
 
    return(0);
}





