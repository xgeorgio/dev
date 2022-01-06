//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.2
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#include <windows.h>
#include <stdio.h>
#include <commdlg.h>
#include <math.h>
#include "generic.h"
#include "defines.h"


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
   static int c, cx, cy, sz, scrstep, imgtop=100, imgleft=100;


   // initialize drawing
   hdc = BeginPaint( hWnd, &ps );
   GetClientRect(hWnd, &r); 

   // typical drawing output in the client window
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0xAA,0x7F,0x0F)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0xAA,0x7F,0x0F)));

   RoundRect(hdc,r.left,30,r.right,40,10,10);

   Rectangle(hdc,500,40,530,r.bottom-100);
   RoundRect(hdc,500,r.bottom-110,530,r.bottom-45,10,10);
   Ellipse(hdc,490,30,510,50);
   Ellipse(hdc,520,30,540,50);

   RoundRect(hdc,520,200,r.right,210,10,10);
   Ellipse(hdc,520,190,540,210);
   Ellipse(hdc,520,200,540,220);

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
   SetTextColor(hdc,RGB(0xCA,0x9F,0x2F));

   sprintf(strBuffer,"%s",VERSION_TAG);
   TextOut(hdc,10,10,strBuffer,strlen(strBuffer));

   // draw graphs
   DeleteObject(pen);
   DeleteObject(brush);
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0xDF,0xDF,0xDF)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0xDF,0xDF,0xDF)));
   SetTextColor(hdc,RGB(0xDF,0xDF,0xDF));

   sprintf(strBuffer,"CFG file: '%s'",appStatus.InpFileName);
   TextOut(hdc,550,60,strBuffer,strlen(strBuffer));  

   TextOut(hdc,10,510,appMesg,strlen(appMesg));

   if (appStatus.drawImage)
   {
       greyscale(hdc,imgleft+xx+30,imgtop);
       imageplot(hdc,imgleft,imgtop,appStatus.imdata);
   }

   if (appStatus.isNetwork) {

       switch (appStatus.datMode) {
          case PAT_NONE: sz=appStatus.maxiter;  sprintf(strBuffer,"Create data: NS"); break;
          case PAT_ASP: sz=appStatus.maxiter*2;  sprintf(strBuffer,"Create data: NS, ASP"); break;
          case PAT_ASM: sz=appStatus.maxiter*2;  sprintf(strBuffer,"Create data: NS, ASM"); break;
          default /*PAT_BOTH*/: sz=appStatus.maxiter*3;  sprintf(strBuffer,"Create data: NS, ASP, ASM"); break;
       }
       TextOut(hdc,550,155,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Image file: '%s'",appStatus.imgFileName);
   TextOut(hdc,550,80,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Box size: %dx%d",appStatus.boxSize,appStatus.boxSize);
   TextOut(hdc,550,95,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Features: %d",MAX_FEATURE);
   TextOut(hdc,550,110,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Noise level: %5.3f",appStatus.nslev);
   TextOut(hdc,550,125,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Noise bias: %5.3f",appStatus.nsbias);
   TextOut(hdc,550,140,strBuffer,strlen(strBuffer));
   
   sprintf(strBuffer,"Patterns file: '%s' (size=%d)",appStatus.patFileName,sz);
   TextOut(hdc,550,170,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Processing sample: %d of %d",appStatus.iter,appStatus.maxiter);
   TextOut(hdc,550,235,strBuffer,strlen(strBuffer));

   if (appStatus.sdat)  {
         DeleteObject(pen);
         pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0x00,0xFF,0x00)));
         for ( c=0; c<appStatus.iter; c++ ) {
           cx=(int)appStatus.sdat[c].x+imgleft;
           cy=(int)appStatus.sdat[c].y+imgtop;
           //Rectangle(hdc,cx,cy,cx+appStatus.boxSize,cy+appStatus.boxSize);
           MoveToEx(hdc,cx,cy,NULL);
           LineTo(hdc,cx+appStatus.boxSize,cy);
           LineTo(hdc,cx+appStatus.boxSize,cy+appStatus.boxSize);
           LineTo(hdc,cx,cy+appStatus.boxSize);
           LineTo(hdc,cx,cy);  }
        }
   }

   // finish drawing
   DeleteObject(pen);
   DeleteObject(brush);

   EndPaint( hWnd, &ps );
}

void ResetStatus( TStatusInfo* status, BOOL clearstats )
{
    if (status)
    {
        if (status->inpfile)  fclose(status->inpfile);
        if (status->logfile)  fclose(status->logfile);

        status->isInpFile=FALSE;
        status->isLogFile=FALSE;

        if (clearstats)
        {
            status->isNetwork=FALSE;
            status->drawImage=FALSE;

            strcpy(status->LogFileName,"");
            strcpy(status->InpFileName,"");
            strcpy(status->patFileName,"");
            strcpy(status->datFileName,"");
            strcpy(status->imgFileName,"");

            status->maxiter=0;  status->iter=0;
            status->boxSize=0;  status->datMode=0;  status->gscale=0;
            if (status->sdat)  { free(status->sdat); status->sdat=NULL; }

            status->nsbias=0;  status->nslev=0;
            status->isNetwork=FALSE;
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

    
//**********************************************************


void fatal( HDC hdc, char *mes )
{
    DrawMessage(hdc,mes);
    MessageBox(NULL, mes, appName, MB_ICONERROR);
    exit(1);
}


//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_CONST_H
#define		_CONST_H


//#define		VERSION_VER		"2.3"
//#define		VERSION_REL		"R000711"

#define     BMP_header      1078

//#define     xx      256
//#define     yy      256

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


#define		round(x)		floor(x+0.5)

//#define		max(x,y)		((x>y)?x:y)

//#define		min(x,y)		((x<y)?x:y)


#endif


//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_FEATURES_H
#define		_FEATURES_H

//#include "const.h"


//----------  IMAGE TEXTURE FEATURE FUNCTIONS  ----------


//... default (null) feature function [CKECKED:OK] ...
double tfc_def( int *img, int sx, int sy )
{
	return(0);
}

//..........  1st ORDER STATISTICS (gray-level) ..........

//... F01: mean grey level [CKECKED:OK] ...
double tfc_f01( int *img, int sx, int sy )
{
	int		i, j;
	double  res=0;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
			res = res + *(img+i+j*sx);

	res = res / (sx*sy);
#ifdef	DEBUG_PRINT
	//printf("F01 value = %g\n",res);
#endif
	return(res);
}

//... F02: variance of grey levels [CKECKED:OK] ...
double tfc_f02( int *img, int sx, int sy, double fv1 )
{
	int		i, j;
	double  res=0, t;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
		{ t = (*(img+i+j*sx) - fv1);  res += t*t; }

	res=res/(sx*sy);

#ifdef	DEBUG_PRINT
	//printf("F02 value = %g\n",res);
#endif
	return(res);
}

//... F03: skewness of grey level distribution [CKECKED:OK] ...
double tfc_f03( int *img, int sx, int sy, double fv1 )
{
	int		i, j;
	double  res=0, s1=0, s2=0, t1, t2;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
		{
			t1 = ((*(img+i+j*sx)) - fv1);
			s1 += t1*t1*t1;

			t2= ((*(img+i+j*sx)) * (*(img+i+j*sx))) - fv1*fv1;
			s2 += t2;
		}

    if (s2<=0)  // variance is zero, so set skewness to zero
    {
#ifdef	DEBUG_PRINT
        //printf("Warning: divide by zero in F03\n");
#endif
        res = 0;
    }
    else
    {
    	s1 = s1/(sx*sy);
        s2 = sqrt(s2);  s2 = s2*s2*s2;
	    res = s1/s2;
    }

#ifdef	DEBUG_PRINT
	//printf("F03 value = %g\n",res);
#endif
	return(res);
}

//... F04: curtosis of grey level distribution [CKECKED:OK] ...
double tfc_f04( int *img, int sx, int sy, double fv1 )
{
	int		i, j;
	double  res=0, s1=0, s2=0, t1, t2;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
		{
			t1 = ((*(img+i+j*sx)) - fv1);
			s1 += t1*t1*t1*t1;

			t2= ((*(img+i+j*sx)) * (*(img+i+j*sx))) - fv1*fv1;
			s2 += t2;
		}

    if (s2<=0)  // variance is zero, so set curtosis to zero
    {
#ifdef	DEBUG_PRINT
        //printf("Warning: divide by zero in F04\n");
#endif
        res = 0;
    }
    else
    {
    	s1 = s1/(sx*sy);
	    s2 = sqrt(s2);  s2 = s2*s2*s2*s2;
	    res = s1/s2;
    }

#ifdef	DEBUG_PRINT
	//printf("F04 value = %g\n",res);
#endif
	return(res);
}

//..........  SPATIAL DEPENDENCE STATISTICS (SDM)  ..........

//... F05: angular second moment [CKECKED:OK] ...
double tfc_f05( int *img, int sx, int sy )
{
	int		i, j;
	double  res=0;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
			res += (*(img+i+j*sx)) * (*(img+i+j*sx));

#ifdef	DEBUG_PRINT
	//printf("F05 value = %g\n",res);
#endif
	return(res);
}

//... F06: contrast [CKECKED:OK] ...
double tfc_f06( int *img, int sx, int sy )
{
	int		i, j, n;
	double  sum=0, res=0;

	for ( n=0; n<=sx-1; n++ )
	{
		sum=0;
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if (abs(i-j)==n)  sum += (*(img+i+j*sx));

		res += n*n*sum;     // always 0 for 1st iteration (n-0)
	}

#ifdef	DEBUG_PRINT
	//printf("F06 value = %g\n",res);
#endif
	return(res);
}

//... F07: correletion [CKECKED:OK] ...
double tfc_f07( int *img, int sx, int sy )
{
	int		i, j, *px=new int[sx], *py=new int[sy];
	double  res=0, mx, my, vx, vy;

	for ( i=0; i<sx; i++ )
		for ( px[i]=0,j=0; j<sy; j++ )	px[i] += (*(img+i+j*sx));

	for ( j=0; j<sy; j++ )
		for ( py[j]=0,i=0; i<sx; i++ )	py[j] += (*(img+i+j*sx));

	for ( mx=0,i=0; i<sx; i++ )  mx += px[i];
	mx = mx / sx;

	for ( my=0,j=0; j<sy; j++ )  my += py[j];
	my = my / sy;

	for ( vx=0,i=0; i<sx; i++ )  vx += (px[i]-mx)*(px[i]-mx);
	vx = sqrt(vx/sx);

	for ( vy=0,j=0; j<sy; j++ )  vy += (py[j]-my)*(py[j]-my);
    vy = sqrt(vy/sy);


	res=0;
    for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
			res += ((i*j) * (*(img+i+j*sx)));

    if ((vx==0)||(vy==0))
    {
#ifdef	DEBUG_PRINT
        //printf("Warning: divide by zero in F07\n");
#endif
        res = 0;
    }
    else
        res = (res - mx*my)/(vx*vy);

	delete px;  delete py;

#ifdef	DEBUG_PRINT
	//printf("F07 value = %g\n",res);
#endif
	return(res);
}

//... F08: sum of squares variance [CKECKED:OK] ...
double tfc_f08( int *img, int sx, int sy, double fv1 )
{
	int		i, j;
	double  res=0, m=fv1;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )	res += (i-m)*(i-m) * (*(img+i+j*sx));

#ifdef	DEBUG_PRINT
	//printf("F08 value = %g\n",res);
#endif
	return(res);
}

//... F09: inverse difference moment [CKECKED:OK] ...
double tfc_f09( int *img, int sx, int sy )
{
	int		i, j;
	double  res=0;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
			res += ((*(img+i+j*sx)) / (1 + (i-j)*(i-j)));

#ifdef	DEBUG_PRINT
	//printf("F09 value = %g\n",res);
#endif
	return(res);
}

//... F10: sum average [CKECKED:OK] ...
double tfc_f10( int *img, int sx, int sy )
{
	int		i, j, k;
	double  pxy=0, res=0;

	for ( k=2; k<2*sx; k++ )
	{
        pxy=0;  // recalculate pxy(k) for current k
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if ((i+j)==k)  pxy += (*(img+i+j*sx));

		res += k*pxy;
	}

#ifdef	DEBUG_PRINT
	//printf("F10 value = %g\n",res);
#endif
	return(res);
}

//... F11: sum variance [CKECKED:OK] ...
double tfc_f11( int *img, int sx, int sy, double fv6 )
{
	int		i, j, k;
	double  pxy=0, res=0;

	for ( k=2; k<2*sx; k++ )
	{
        pxy=0;
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if ((i+j)==k)  pxy += (*(img+i+j*sx));

		res += (k-fv6)*(k-fv6)*pxy;
	}

#ifdef	DEBUG_PRINT
	//printf("F11 value = %g\n",res);
#endif
	return(res);
}

//... F12: sum entropy [CKECKED:OK] ...
double tfc_f12( int *img, int sx, int sy )
{
	int		i, j, k;
	double  pxy=0, res=0;

	for ( k=2; k<2*sx; k++ )
	{
        pxy=0;
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if ((i+j)==k)  pxy += (*(img+i+j*sx));

        if (pxy<LOGMINX)  pxy = LOGMINX;
		res += pxy*log10(pxy);
	}
	res = -res;

#ifdef	DEBUG_PRINT
	//printf("F12 value = %g\n",res);
#endif
	return(res);
}

//... F13: entropy [CKECKED:OK] ...
double tfc_f13( int *img, int sx, int sy )
{
	int		i, j;
	double  res=0, t;

	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
		{
			t = (*(img+i+j*sx));
			if (t<LOGMINX)  t = LOGMINX;
			res += t*log10(t);
		}
	res = -res;

#ifdef	DEBUG_PRINT
	//printf("F13 value = %g\n",res);
#endif
	return(res);
}

//... F14: dirrerence variance [CKECKED:OK] ...
double tfc_f14( int *img, int sx, int sy, double fv6 )
{
	int		i, j, k;
	double  pxy=0, res=0;

	for ( k=0; k<sx; k++ )
	{
        pxy=0;
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if (abs(i-j)==k)  pxy += (*(img+i+j*sx));

		res += (k-fv6)*(k-fv6)*pxy;     // use squared diffs (see F11)
	}

#ifdef	DEBUG_PRINT
	//printf("F14 value = %g\n",res);
#endif
	return(res);
}


//... F15: difference entropy [CKECKED:OK] ...
double tfc_f15( int *img, int sx, int sy )
{
	int		i, j, k;
	double  pxy=0, res=0;

	for ( k=0; k<sx; k++ )
	{
        pxy=0;
		for ( i=0; i<sx; i++ )
			for ( j=0; j<sy; j++ )
				if (abs(i-j)==k)  pxy += (*(img+i+j*sx));

		if (pxy<LOGMINX)  pxy = LOGMINX;
		res += pxy*log10(pxy);
	}
	res = -res;

#ifdef	DEBUG_PRINT
	//printf("F15 value = %g\n",res);
#endif
	return(res);
}

//..........  RUN-LENGTH STATISTICS (RLM)  ..........

//... F16: short runs emphasis (RF1) [CKECKED:OK] ...
double tfc_f16( int *rlm, int gsz, int rsz )
{
	int		i, j;
	double  sum1=0, sum2=0, res=0;

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum1 += (*(rlm+i*rsz+j))/((double)(j+1)*(j+1));

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum2 += (*(rlm+i*rsz+j));

	if (sum2==0) { printf("Warning: RF1 division by zero\n"); res=sum1; }
	else  res=sum1/sum2;

#ifdef	DEBUG_PRINT
	//printf("F16 value = %g\n",res);
#endif
	return(res);
}


//... F17: long runs emphasis (RF2) [CKECKED:OK] ...
double tfc_f17( int *rlm, int gsz, int rsz )
{
	int		i, j;
	double  sum1=0, sum2=0, res=0;

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum1 += (*(rlm+i*rsz+j))*((double)(j+1)*(j+1));

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum2 += (*(rlm+i*rsz+j));

	if (sum2==0) { printf("Warning: RF2 division by zero\n"); res=sum1; }
	else  res=sum1/sum2;

#ifdef	DEBUG_PRINT
	//printf("F17 value = %g\n",res);
#endif
	return(res);
}


//... F18: gray level non-uniformity (RF3) [CKECKED:OK] ...
double tfc_f18( int *rlm, int gsz, int rsz )
{
	int		i, j;
	double  sum1=0, ssum1=0, sum2=0, res=0;

	for ( i=0; i<gsz; i++ )
	{
		ssum1=0;
		for ( j=0; j<rsz; j++ )	ssum1 += (*(rlm+i*rsz+j));
		sum1 += ssum1*ssum1;
	}

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum2 += (*(rlm+i*rsz+j));

	if (sum2==0) { printf("Warning: RF3 division by zero\n"); res=sum1; }
	else  res=sum1/sum2;

#ifdef	DEBUG_PRINT
	//printf("F18 value = %g\n",res);
#endif
	return(res);
}


//... F19: run length non-uniformity (RF4) [CKECKED:OK] ...
double tfc_f19( int *rlm, int gsz, int rsz )
{
	int		i, j;
	double  sum1=0, ssum1=0, sum2=0, res=0;

	for ( j=0; j<rsz; j++ )
	{
		ssum1=0;
		for ( i=0; i<gsz; i++ )	ssum1 += (*(rlm+i*rsz+j));
		sum1 += ssum1*ssum1;
	}

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			sum2 += (*(rlm+i*rsz+j));

	if (sum2==0) { printf("Warning: RF4 division by zero\n"); res=sum1; }
	else  res=sum1/sum2;

#ifdef	DEBUG_PRINT
	//printf("F19 value = %g\n",res);
#endif
	return(res);
}


//... F20: run percentage (RF5) [CKECKED:OK] ...
double tfc_f20( int *rlm, int gsz, int rsz )
{
	int		i, j;
	double  dival=gsz*rsz, res=0;

	if (dival==0) { printf("Warning: RF5 division by zero\n"); return(0); }

	for ( i=0; i<gsz; i++ )
		for ( j=0; j<rsz; j++ )
			res += (*(rlm+i*rsz+j));
    res = res/dival;    // range: from 'rsz/gsz' (all runs=1) to '1' (uniform image lines)

#ifdef	DEBUG_PRINT
	//printf("F20 value = %g\n",res);
#endif
	return(res);
}


//..... global feature calculator .....
void calcfeatures( int *img, int ix, int iy, int *sdm, int sx, int sy, int *rlm, int gsz, int rsz, double *FeatValue )
{
	int		c;

	for ( c=0; c<MAX_FEATURE; c++ )  FeatValue[c]=0;

    //... calculate 1st order features ...
    FeatValue[0]=tfc_f01(img,ix,iy);
	FeatValue[1]=tfc_f02(img,ix,iy,FeatValue[0]);
	FeatValue[2]=tfc_f03(img,ix,iy,FeatValue[0]);
	FeatValue[3]=tfc_f04(img,ix,iy,FeatValue[0]);
	
    //... calculate SDM features ...
	FeatValue[4]=tfc_f05(sdm,sx,sy);
	FeatValue[5]=tfc_f06(sdm,sx,sy);
	FeatValue[6]=tfc_f07(sdm,sx,sy);
	FeatValue[7]=tfc_f08(sdm,sx,sy,FeatValue[0]);
	FeatValue[8]=tfc_f09(sdm,sx,sy);
	FeatValue[9]=tfc_f10(sdm,sx,sy);
	FeatValue[10]=tfc_f11(sdm,sx,sy,FeatValue[5]);
	FeatValue[11]=tfc_f12(sdm,sx,sy);
	FeatValue[12]=tfc_f13(sdm,sx,sy);
	FeatValue[13]=tfc_f14(sdm,sx,sy,FeatValue[5]);
	FeatValue[14]=tfc_f15(sdm,sx,sy);

    //... calculate RLM features ...
	FeatValue[15]=tfc_f16(rlm,gsz,rsz);
	FeatValue[16]=tfc_f17(rlm,gsz,rsz);
	FeatValue[17]=tfc_f18(rlm,gsz,rsz);
	FeatValue[18]=tfc_f19(rlm,gsz,rsz);
	FeatValue[19]=tfc_f20(rlm,gsz,rsz);
}


#endif


//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


//#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <float.h>
#include <time.h>
//#include "const.h"
//#include "features.h"
//#include <conio.h>


typedef struct tagPoint {
	int		x, y;		} TPoint;

typedef struct tagPtvalue {
	int		pt;
	double	val;		} TPtvalue;


//.....................................

int *readimage( HDC hdc, char *filename )
{
    int		header[BMP_header];
	FILE	*fp;
    int		i, j, *im;

    im=new int[xx*yy];
	if (!im)  return(NULL);

    fp=fopen(filename,"rb");
    if (fp==NULL)
    {  fatal(hdc,"Error: Unable to open image file"); return(NULL);  }

    for ( i=0; i<BMP_header; i++ )  { header[i]=getc(fp); }

    // XDist=header[18]+256*(header[19]+256*(header[20]+256*header[21]));
    // YDist=header[22]+256*(header[23]+256*(header[24]+256*header[25]));

    for ( i=0; i<yy; i++ )  {
        for ( j=0; j<xx; j++ )  {
            *(im+j+i*xx)=(unsigned int)getc(fp);
        }
    }

    fclose(fp);

    return(im);
}

//... Normalize image & scale to current greylevel range ...
int *gsn_image( int *im, int gscale )
{
    int     i, j, minv, maxv, rngv, *gs_im;

	gs_im=new int[xx*yy];
	if (!gs_im)  return(NULL);

    minv=*im;  maxv=*im;
    for ( i=0; i<xx; i++ )
        for ( j=0; j<yy; j++ )
        {
            if (*(im+i+j*xx) < 0)  *(im+i+j*xx) = - *(im+i+j*xx);
            if (*(im+i+j*xx) < minv)  minv=*(im+i+j*xx);
            if (*(im+i+j*xx) > maxv)  maxv=*(im+i+j*xx);
        }

	rngv=maxv-minv;
	if (rngv<=0)  rngv=1;

    for ( i=0; i<xx; i++ )
        for ( j=0; j<yy; j++ )
		{
            *(gs_im+i+j*xx) = (int)((*(im+i+j*xx)-minv)*(double)(gscale-1)/rngv);
		}
    
    return(gs_im);
}

//... Copy image region to sample box ...
int copysample(int *img, int cx, int cy, int sx, int sy, int *box)
{
    int     i, j;

	if ((cx+sx>xx)||(cy+sy>yy))
	{
#ifdef	DEBUG_PRINT
		//printf("Error: sample box out of range\n");
#endif
		return(1);
	}

    for ( i=0; i<sx; i++ )
        for ( j=0; j<sy; j++ )
            *(box+i+j*sx) = *(img+(cx+i)+(cy+j)*xx);

	return(0);
}

//... Calculate Spatial Dependence Matrix (SDM) for image region ...
int *calculate_sdm( HDC hdc, int *box_dat, int sx, int sy, int gsc )
{
	int		i, j, sdm_dirs=0, pgv, cgv;
	int		*box_sdm=new int[gsc*gsc];


	if (!box_sdm)
	{
		fatal(hdc,"Unable to allocate SDM buffer");
		return(NULL);
	}

	//... initialize SDM matrices ...
#ifdef	DEBUG_PRINT
	//printf("calc.SDM...");
#endif

	for ( i=0; i<gsc; i++ )
		for ( j=0; j<gsc; j++ )
			*(box_sdm+i+j*gsc)=0;

	//... SDM calculations for 'N->S' direction ...
#ifdef	DEBUG_PRINT
	//printf("[N:S");
#endif

	sdm_dirs++;

	for ( i=0; i<sx; i++ )
	{
		for ( j=1; j<sy; j++ )
		{
			pgv = *(box_dat+i+(j-1)*sx);
			cgv = *(box_dat+i+j*sx);
			if ((pgv<0)||(pgv>=gsc)||(cgv<0)||(cgv>=gsc))
			{
				printf("Error: SDM index out of range\n");
				delete box_sdm;  return(NULL);
			}
			(*(box_sdm + pgv + cgv*gsc))++;
		}
	}

	//... SDM calculations for 'W->E' direction ...
#ifdef	DEBUG_PRINT
	//printf(",W:E]...");
#endif

	sdm_dirs++;

	for ( j=0; j<sy; j++ )
	{
    	for ( i=1; i<sx; i++ )
	    {
			pgv = *(box_dat+(i-1)+j*sx);
			cgv = *(box_dat+i+j*sx);
			if ((pgv<0)||(pgv>=gsc)||(cgv<0)||(cgv>=gsc))
			{
                MessageBox(NULL, "SDM index out of range", appName, MB_ICONEXCLAMATION);
				delete box_sdm;  return(NULL);
			}
			(*(box_sdm + pgv + cgv*gsc))++;
		}
	}

	//... Averaging SDM calculations for all directions ...
#ifdef	DEBUG_PRINT
	//printf("[avg]...");
#endif

	for ( i=0; i<gsc; i++ )
		for ( j=0; j<gsc; j++ )
			*(box_sdm+i+j*gsc)  = (int)(*(box_sdm+i+j*gsc)/(double)sdm_dirs);

#ifdef	DEBUG_PRINT
	//printf("done.\n");
#endif

	return(box_sdm);
}


//... Calculate Run-Length Matrix (RLM) for image region ...
int *calculate_rlm( HDC hdc, int *box_dat, int sx, int sy, int gsc )
{
	int		i, j, rlm_dirs=0, pgv, cgv, crl, boxsz=max(sx,sy);
	int		*box_rlm=new int[gsc*boxsz];


	if (!box_rlm)
	{
		fatal(hdc,"Unable to allocate RLM buffer");
		return(NULL);
	}

	//... initialize RLM matrices ...
#ifdef	DEBUG_PRINT
	//printf("calc.RLM...");
#endif

	for ( j=0; j<gsc; j++ )
		for ( i=0; i<boxsz; i++ )
			*(box_rlm+i+j*boxsz)=0;

	//... RLM calculations for 'N->S' direction ...
#ifdef	DEBUG_PRINT
	//printf("[N:S");
#endif

	rlm_dirs++;

	for ( i=0; i<sx; i++ )
	{
		for ( j=1,crl=0; j<sy; j++ )
		{
    		pgv = *(box_dat+i+(j-1)*sx);
			cgv = *(box_dat+i+j*sx);
            if (pgv==cgv)  crl++;
			else
			{
				if ((pgv<0)||(pgv>=gsc)||(crl<0)||(crl>=boxsz))
				{
                    MessageBox(NULL, "RLM index out of range", appName, MB_ICONEXCLAMATION);
					delete box_rlm;  return(NULL);
				}
				(*(box_rlm+crl+pgv*boxsz))++;
				crl=0;
			}
		}
		if (crl)  (*(box_rlm+crl+pgv*boxsz))++;
	}

	//... RLM calculations for 'W->E' direction ...
#ifdef	DEBUG_PRINT
	//printf(",W:E]...");
#endif

	rlm_dirs++;

	for ( j=0; j<sy; j++ )
	{
    	for ( i=1,crl=0; i<sx; i++ )
	    {
			pgv = *(box_dat+(i-1)+j*sx);
			cgv = *(box_dat+i+j*sx);
			if (pgv==cgv)  crl++;
			else
			{
				if ((pgv<0)||(pgv>=gsc)||(crl<0)||(crl>=boxsz))
				{
                    MessageBox(NULL, "RLM index out of range", appName, MB_ICONEXCLAMATION);
					delete box_rlm;  return(NULL);
				}
				(*(box_rlm+crl+pgv*boxsz))++;
				crl=0;
			}
		}
		if (crl)  (*(box_rlm+crl+pgv*boxsz))++;
	}

	//... Averaging RLM calculations for all directions ...
#ifdef	DEBUG_PRINT
	//printf("[avg]...");
#endif

	for ( j=0; j<gsc; j++ )
		for ( i=0; i<boxsz; i++ )
			*(box_rlm+i+j*boxsz)=(int)(*(box_rlm+i+j*boxsz)/(double)rlm_dirs);
	
#ifdef	DEBUG_PRINT
	//printf("done.\n");
#endif
	
	return(box_rlm);
}
	
	
//... create normal & abnormal samples, calculate features ...
int createpatterns( HDC hdc, FILE *fout, int *ns_dat, int sx, int sy, int gscale, double noiselev, double bias, int pat_types )
{
	int		i, j, boxsz=max(sx,sy);
    int     asp_dat[MAX_BOXSIZE*MAX_BOXSIZE], asm_dat[MAX_BOXSIZE*MAX_BOXSIZE];
    int     *ns_sdm, *asp_sdm, *asm_sdm, sdm_dirs=0;
	int		*ns_rlm, *asp_rlm, *asm_rlm, rlm_dirs=0, crl=0;
	double	nsval, bsval, FeatValue[MAX_FEATURE];


	//... Noisy sampler: create normal & abnormal (+/-) boxes ...
#ifdef	DEBUG_PRINT
	//printf("\tNew pattern(s): sampling...");
#endif

    // Warning: if called in short time, seed is same (so is rand() sequences)!!!
	srand( (unsigned)time( NULL ) );

	//bsval=round(rand()*bias/RAND_MAX*gscale);		//-> dynamic noise bias
	bsval=fabs(round(bias*gscale));		//-> static noise bias
	for ( i=0; i<sx; i++ )
		for ( j=0; j<sy; j++ )
		{
			nsval=fabs(round(rand()*noiselev/RAND_MAX*gscale));

            if (pat_types & PAT_ASP)
            {
                *(asp_dat+i+j*sx) = (int)( *(ns_dat+i+j*sx)+bsval+nsval );
			    if (*(asp_dat+i+j*sx)>(gscale-1))  *(asp_dat+i+j*sx)=gscale-1;
			    else if (*(asp_dat+i+j*sx)<0)  *(asp_dat+i+j*sx)=0;
            }

            if (pat_types & PAT_ASM)
            {
                *(asm_dat+i+j*sx) = (int)( *(ns_dat+i+j*sx)-bsval-nsval );
			    if (*(asm_dat+i+j*sx)>(gscale-1))  *(asm_dat+i+j*sx)=gscale-1;
			    else if (*(asm_dat+i+j*sx)<0)  *(asm_dat+i+j*sx)=0;
            }

#ifdef	DEBUG_PRINT
			//printf("Value = %d",*(ns_dat+i+j*sx));
			//if (pat_types & PAT_ASP) printf(" , Noisy(+) = %d (%+4.2f%%)",*(asp_dat+i+j*sx),nsval*100);
			//if (pat_types & PAT_ASM) printf(" , Noisy(-) = %d (%+4.2f%%)\n",*(asm_dat+i+j*sx),-nsval*100);
            //printf("\n");
#endif
		}

	//***** calculate spatial dependence matrices (SDM) for samples *****
	if ((ns_sdm=calculate_sdm(hdc,ns_dat,sx,sy,gscale))==NULL)
	{
        MessageBox(NULL, "Error calculating SDM for 'ns' sample.", appName, MB_ICONEXCLAMATION);
		return(1);
	}
	
	if (pat_types & PAT_ASP)
        if ((asp_sdm=calculate_sdm(hdc,asp_dat,sx,sy,gscale))==NULL)
	    {
            MessageBox(NULL, "Error calculating SDM for 'asp' sample.", appName, MB_ICONEXCLAMATION);
		    return(1);
	    }

	if (pat_types & PAT_ASM)
        if ((asm_sdm=calculate_sdm(hdc,asm_dat,sx,sy,gscale))==NULL)
	    {
            MessageBox(NULL, "Error calculating SDM for 'asm' sample.", appName, MB_ICONEXCLAMATION);
		    return(1);
	    }

	//***** calculate run-length matrices (RLM) for samples *****
	if ((ns_rlm=calculate_rlm(hdc,ns_dat,sx,sy,gscale))==NULL)
	{
        MessageBox(NULL, "Error calculating RLM for 'ns' sample.", appName, MB_ICONEXCLAMATION);
		return(1);
	}
	
	if (pat_types & PAT_ASP)
	    if ((asp_rlm=calculate_rlm(hdc,asp_dat,sx,sy,gscale))==NULL)
	    {
            MessageBox(NULL, "Error calculating RLM for 'asp' sample.", appName, MB_ICONEXCLAMATION);
		    return(1);
	    }

	if (pat_types & PAT_ASM)
	    if ((asm_rlm=calculate_rlm(hdc,asm_dat,sx,sy,gscale))==NULL)
	    {
            MessageBox(NULL, "Error calculating RLM for 'asm' sample.", appName, MB_ICONEXCLAMATION);
		    return(1);
	    }

	//***** Calculate features for all samples *****
#ifdef	DEBUG_PRINT
	//printf("done\nCalculating features...[ns]...");
#endif
	// calculate features for normal (ns) box
	calcfeatures(ns_dat,sx,sy,ns_sdm,gscale,gscale,ns_rlm,gscale,boxsz,FeatValue);
	fprintf(fout,"%3.1f\t",0.0);
	for ( i=0; i<MAX_FEATURE; i++ )  fprintf(fout,"%8.3f\t",FeatValue[i]);
	fprintf(fout,"\n");

	if (pat_types & PAT_ASP)
    {
#ifdef	DEBUG_PRINT
	    //printf("[asp]...");
#endif
        // calculate features for noisy (asp) box
	    calcfeatures(asp_dat,sx,sy,asp_sdm,gscale,gscale,asp_rlm,gscale,boxsz,FeatValue);
	    fprintf(fout,"%3.1f\t",1.0);
	    for ( i=0; i<MAX_FEATURE; i++ )  fprintf(fout,"%8.3f\t",FeatValue[i]);
	    fprintf(fout,"\n");
    }

	if (pat_types & PAT_ASM)
    {
#ifdef	DEBUG_PRINT
    	//printf("[asm]...");
#endif
	    // calculate features for noisy (asp) box
	    calcfeatures(asm_dat,sx,sy,asm_sdm,gscale,gscale,asm_rlm,gscale,boxsz,FeatValue);
	    fprintf(fout,"%3.1f\t",1.0);
	    for ( i=0; i<MAX_FEATURE; i++ )  fprintf(fout,"%8.3f\t",FeatValue[i]);
	    fprintf(fout,"\n");
    }

#ifdef	DEBUG_PRINT
	//printf("done\n");
#endif

	delete ns_sdm;  delete ns_rlm;
	if (pat_types & PAT_ASP)  { delete asp_sdm; delete asp_rlm; }
    if (pat_types & PAT_ASM)  { delete asm_sdm; delete asm_rlm; }

	return(0);
}

//... transfer patterns to output file (add header data) ...
long convertpatternfile( FILE *ffrom, FILE *fto )
{
	long	count=0;
	char	str[MAX_LINESIZE];
	int		res=1;


	rewind(ffrom);
	while (fgets(str,MAX_LINESIZE,ffrom))  count++;

	rewind(ffrom);  //rewind(fto);
	fprintf(fto,"%ld %d\n",count,MAX_FEATURE+1);
	while (fgets(str,MAX_LINESIZE,ffrom))  //res = res & fputs(str,fto);
		fprintf(fto,"%s",str);

	if (res)  return(count);
	else  return(0);
}

//... write patterns to file ...
long writepatternfile( double *pattern, long patlen, long patsize, FILE *fto )
{
	long	pc, fc;

	rewind(fto);
	//fprintf(fto,"NORMALIZED PATTERNS FILE\n");
	fprintf(fto,"%ld %d\n",patlen,(int)patsize);
	for ( pc=0; pc<patlen; pc++ )
	{
		for ( fc=0; fc<patsize; fc++ )  fprintf(fto,"%8.3f\t",*(pattern+fc+pc*patsize));
		fprintf(fto,"\n");
	}

	return(pc);
}

//... augmented output file for use with ANN32 package ...
long writeauxfile0( double *pattern, long patlen, long patsize, int clscount, FILE *fto )
{
	long	pc, fc;
	double	cls;

	for ( pc=0; pc<patlen; pc++ )
	{
		cls = *(pattern+0+pc*patsize);
		for ( fc=1; fc<patsize; fc++ )
			fprintf(fto,"%8.3f\t",*(pattern+fc+pc*patsize));
		fprintf(fto,"\n");
		for ( fc=0; fc<clscount; fc++ )
		{
			if (fc==(int)cls)  fprintf(fto,"%5.3f\t",(double)1.0);
			else  fprintf(fto,"%5.3f\t",(double)0.0);
		}
		fprintf(fto,"\n");
	}

	return(pc);
}

//... read patterns from file ...
double* readpatterns( HDC hdc, FILE *ffrom, long *patlen, long *patsize )
{
	long	count=0, pc=0, nrm_fc=0, abn_fc=0;
	char    sval[MAX_LINESIZE];
	int		t, currf=0, fn, ok=1;
	double	dval, *pattern;


	rewind(ffrom);
    fscanf(ffrom,"%ld%d",&count,&fn);
	
	if ((count<=0)||(fn<=0))  return(0);

	*patlen=count;  *patsize=fn;

	pattern=new double[count*fn];
	if (!pattern)
	{
        fatal(hdc,"Unable to allocate patterns buffer.");
		if (pattern)  delete pattern;
		return(0);
	}

	for ( pc=0; pc<count; pc++ )
		for ( t=0; t<fn; t++ )
		{
			fscanf(ffrom,"%s",sval);  dval=strtod(sval,NULL);
			if ((dval==HUGE_VAL)||(dval==-HUGE_VAL)||(_isnan(dval)))
				printf("Warning: pattern[%ld][%d] = NAN  (%g)\n",pc,t,dval);
			pattern[pc*(MAX_FEATURE+1)+t]=dval;
		}

	//printf("\npatterns read:  count=%ld , features=%d\n",pc,t-1);

	if (ok)  return(pattern);
	else  { delete pattern; return(NULL); }
}

//... normalize pattern vectors using: (x-m)/s ...
long normpatterns( double *pattern, long patlen, long patsize )
{
	long	pc=0, fc=0, n;
	double	*mval, *sval;

	mval=new double[patsize];
	sval=new double[patsize];
	if ((!mval)||(!sval))
	{
		if (mval) delete mval;  if (sval) delete sval;  return(0);
	}

	if (patlen<1)  return(0);
	else if (patlen==1)  n=1;
	else n=patlen-1;

	for ( fc=1; fc<patsize; fc++ )
	{
		mval[fc]=0;  sval[fc]=0;

		for ( pc=0; pc<patlen; pc++ )
			mval[fc] = (pc*mval[fc]+pattern[pc*(MAX_FEATURE+1)+fc])/(pc+1);  // on-line mean value calculation, prevent sum overflow
			//mval[fc] += pattern[pc*(MAX_FEATURE+1)+fc];		// classic mean value formula
		mval[fc] = mval[fc]/patsize;

		for ( pc=0; pc<patlen; pc++ )
			sval[fc] += (mval[fc]-pattern[pc*(MAX_FEATURE+1)+fc])*(mval[fc]-pattern[pc*(MAX_FEATURE+1)+fc]);
		
		sval[fc]=sqrt(sval[fc]/n);

		if (sval[fc]>0)
		{
			for ( pc=0; pc<patlen; pc++ )
				pattern[pc*(MAX_FEATURE+1)+fc] = (pattern[pc*(MAX_FEATURE+1)+fc]-mval[fc])/sval[fc];
		}
		//else
		//	printf("\nWarning: unable to normalize feature %ld\n",fc);
	}

	delete mval;  delete sval;
	return(fc);
}

//... normalize pattern vectors using: x/|xmax-xmin| ...
long normpatterns2( double *pattern, long patlen, long patsize )
{
	long	pc=0, fc=0;
	double	*minval, *maxval, denom;

	minval=new double[patsize];
	maxval=new double[patsize];
	if ((!minval)||(!maxval))
	{
		if (minval) delete minval;  if (maxval) delete maxval;  return(0);
	}

	for ( fc=1; fc<patsize; fc++ )
	{
		minval[fc]=FLT_MAX;  maxval[fc]=-FLT_MAX;

		for ( pc=0; pc<patlen; pc++ )
		{
			if (pattern[pc*(MAX_FEATURE+1)+fc]<minval[fc])  minval[fc]=pattern[pc*(MAX_FEATURE+1)+fc];
			else if (pattern[pc*(MAX_FEATURE+1)+fc]>maxval[fc])  maxval[fc]=pattern[pc*(MAX_FEATURE+1)+fc];
		}

		denom=fabs(maxval[fc]-minval[fc]);
		if (denom>0)
		{
			for ( pc=0; pc<patlen; pc++ )
				pattern[pc*(MAX_FEATURE+1)+fc] = pattern[pc*(MAX_FEATURE+1)+fc]/denom;
		}
		//else
		//	printf("\nWarning: unable to normalize feature %ld\n",fc);
	}

	delete minval;  delete maxval;
	return(fc);
}


//.........................................

void NEAR PASCAL greyscale( HDC hdc, int xst, int yst )
{
  int i,j,x,y;
  
  //xst=325+10; yst=10;
  Rectangle(hdc,xst-1,yst-1,xst+22,256+yst+1);
  
  MoveToEx(hdc, xst, yst,NULL);
  for (i=0;i<256;i++) {
    y=yst+i;
    for (j=0;j<=20;j++) {
      x=xst+j;
      SetPixel (hdc, x, y, RGB(i, i, i));
    }
  }
}


int *imageplot(HDC hdc, int x, int y, int *im)
{
  int i,j,color;float max;
  HPEN hpen, hpenOld;


  hpen=GetStockObject(1);
  hpenOld=SelectObject(hdc, hpen);

  Rectangle(hdc,x-1,y-1,x+N+2,y+N+2);
  SelectObject(hdc, hpenOld);
  DeleteObject(hpen);

  max=(float)0.0;
  for (i=0;i<yy;i++) {
	  for (j=0; j<xx; j++) {
	     if( *(im+j+i*xx)>max) max=(float)*(im+j+i*xx);
	
	  }
    }

  for (i=0;i<yy;i++) {
      for (j=0; j<xx; j++) {
	    color=(int)( (255.0 * *(im+j+i*xx )) / max );
	    //color=(BYTE)(*(im+j+i*xx ));
    	SetPixel (hdc, j+x, (yy-i)+y, RGB(color,color,color));		
      }
  }
  return(im);
  
}





//-------------------------------------------

int console_main( HWND hWnd )
{
    int     *imdata, *img, *box;
	TPoint	cnt, bsz;
	char	bmpfname[30], patfname[30], samfname[30];
	float	noisev,bias;
	FILE	*pfh, *ftmp, *sfh, /**cfh,*/ *aux;

	int		gscale, minsel=0, pat_types=0;;
	//int		*FeatComb;
	//int		*FeatConfig;
	double	*pattern;

	long	tcombs=0, fcombs=0, wcombs=0, scount=0, c, sc=0, patlen=0, patsz=0;
    PAINTSTRUCT ps;
    HDC hdc;
    char    msg[256];


    // initialize drawing
    hdc = BeginPaint( hWnd, &ps );
      
	//printf("SIFX: Sample Image Feature Extractor, v%s (%s) - Harris Georgiou (c) 2000\n",VERSION_VER,VERSION_REL);
    //printf("-------------------------------------------------------------------------------\n\n");
    

//... phase 0: initialization ...

	//... initialize image buffers ...
	box=new int[MAX_BOXSIZE*MAX_BOXSIZE];
	if (!box)
	{
		fatal(hdc,"Unable to allocate image buffers.");
		if (box)  delete box;
		return(1);
	}

	//... read bitmap and create normalized greyscaled image ...
	//printf("Give input (image) filename: ");
    fscanf(appStatus.inpfile,"%s",bmpfname);
    if ((imdata=readimage(hdc,bmpfname))==NULL)
    {
		fatal(hdc,"Unable to open image file.");
        return(1);
    }
    strcpy(appStatus.imgFileName,bmpfname);
    // Warning: memcpy() does not copy all 256*256 (64K) bytes!!!!!!!
    //memcpy(appStatus.imdata,imdata,xx*yy);
    for ( c=0; c<xx*yy; c++ )  appStatus.imdata[c]=*(imdata+c);
    appStatus.drawImage=TRUE;
    DrawMessage(hdc,"Image read OK");
    appStatus.drawImage=FALSE;
    
	//printf("Give image scale range (2-%d): ",GSCALE_VALUES);
    fscanf(appStatus.inpfile,"%d",&gscale);
	if (gscale<2)
    {
        gscale=2;
        //printf("Warning: image range scale changed to %d\n",gscale);
    }
	else if (gscale>GSCALE_VALUES)
    {
        gscale=GSCALE_VALUES;
        //printf("Warning: image range scale changed to %d\n",gscale);
    }
    if ((img=gsn_image(imdata,gscale))==NULL)
    {
		fatal(hdc,"Unable to normalize image.");
        return(1);
    }
    DrawMessage(hdc,"Image normalized OK");
    appStatus.gscale=gscale;

	//... read sample points and box size ...
	//printf("Give samples pt file: ");
    fscanf(appStatus.inpfile,"%s",samfname);
	if ((sfh=fopen(samfname,"r"))==NULL)
	{ 
		fatal(hdc,"Unable to open samples file");
		return(2);
	}
		
	//... open & prepare sample pt data file ...
	fscanf(sfh,"%ld%d",&scount,&bsz.x);
	if ((bsz.x<0)||(bsz.x>MAX_BOXSIZE))
	{
		bsz.x=MAX_BOXSIZE;
		//printf("Warning: sample image window changed to %dx%d pts\n",bsz.x,bsz.x);
	}
	bsz.y=bsz.x;
	sprintf(msg,"Creating: %ldx[%dx%d] image samples",scount,bsz.x,bsz.y);
    DrawMessage(hdc,msg);
    strcpy(appStatus.imgFileName,bmpfname);
    appStatus.boxSize=bsz.x;

    if ((appStatus.sdat=(POINT*)calloc(scount,sizeof(POINT)))==NULL)
        fatal(hdc,"Not enough memory for sample data.");
    else
    {
        appStatus.maxiter=(int)scount;
        appStatus.iter=0;
    }

	//... create main patterns file ...
	//printf("Give output patterns filename: ");
    fscanf(appStatus.inpfile,"%s",patfname);
	if ((pfh=fopen(patfname,"w+"))==NULL)
	{ 
		fatal(hdc,"Unable to create output file");
		return(3);
	}
    strcpy(appStatus.patFileName,patfname);

	//... create temporaty (work) patterns file ...
	if ((ftmp=fopen(TMPFILENAME,"w+"))==NULL)
	{ 
		fatal(hdc,"Unable to create work file");
		return(4);
	}

	//... get noise parameters for simulated data ...
	//printf("Give noise bias (0.0 ... +1.0): ");
    fscanf(appStatus.inpfile,"%f",&bias);
	//printf("bias=%g\n",bias);
	//if (bias<0)  bias=-bias;
    appStatus.nsbias=bias;
	
	//printf("Give noise level (0.0 ... +1.0): ");
    fscanf(appStatus.inpfile,"%f",&noisev);
	//printf("noisev=%g\n",noisev);
	//if (noisev<0)  noisev=-noisev;
    appStatus.nslev=noisev;
	
    //printf("Enter type(s) of simulated data to create:\n");
    //printf("\t(%d=NONE, %d=ASP, %d=ASM, %d=BOTH):  ",PAT_NONE,PAT_ASP,PAT_ASM,(PAT_ASP|PAT_ASM));
    fscanf(appStatus.inpfile,"%d",&pat_types);
    switch(pat_types)
    {
        case PAT_NONE:  sprintf(msg,"Simulated data type is: NONE"); break;
        case PAT_ASP:   sprintf(msg,"Simulated data type is: ASP"); break;
        case PAT_ASM:   sprintf(msg,"Simulated data type is: ASM"); break;
        case PAT_BOTH:  sprintf(msg,"Simulated data type is: BOTH"); break;
        //case (PAT_ASP|PAT_ASM):  break;
        default: //printf("Warning: type changed to default (%d)\n",PAT_ASP);
                 pat_types=PAT_BOTH; break;
    }
    //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
    appStatus.datMode=pat_types;

	
//... phase 1: read image samples & calculate features ...

    appStatus.isNetwork=TRUE;

	//... create normal & simulated data, store into work file ...
	//DrawMessage("Creating training patterns...");
    appStatus.drawImage=FALSE;
	while ((sc<scount)&&(!feof(sfh)))
	{
    	sprintf(msg,"Creating training patterns... (%5.1f%%)",sc*100.0/scount);
        DrawMessage(hdc,msg);

		fscanf(sfh,"%d%d",&(cnt.x),&(cnt.y));

		if ((cnt.x<0)||(cnt.y<0))  continue;

		copysample(img,cnt.x,cnt.y,bsz.x,bsz.y,box);
		createpatterns(hdc,ftmp,box,bsz.x,bsz.y,gscale,noisev,bias,pat_types);

        appStatus.sdat[appStatus.iter].x=cnt.x;
        appStatus.sdat[appStatus.iter].y=cnt.y;
        appStatus.iter++;

        sc++;
	}
    //printf("\n");


//... phase 2: construct patterns file (full feature set) ...
	
	//... convert work file into main patterns file ...
	DrawMessage(hdc,"Constructing patterns file...");
	if (convertpatternfile(ftmp,pfh)==0)
        fatal(hdc,"Unable to write to output file");
	//else  printf(" done.\n");

	fclose(ftmp);  remove(TMPFILENAME);

	//... read & normalize main pattern file ...
	DrawMessage(hdc,"Reading initial patterns file...");
	if ((pattern=readpatterns(hdc,pfh,&patlen,&patsz))==0)
        fatal(hdc,"Unable to read patterns file");
	else
	{
		//printf(" done.\n");
		DrawMessage(hdc,"Writing normalized patterns file...");
		if (normpatterns(pattern,patlen,patsz)==0)
			DrawMessage(hdc,"Warning: unable to normalize feature vectors.");
		//else
		//	printf(" done.\n");
	}
	
	fclose(pfh);
	
	//... write final (normalized) patterns file ...
	if ((ftmp=fopen(TMPFILENAME,"w+"))==NULL)
	{ 
		fatal(hdc,"Unable to create work file");
		return(4);
	}

	DrawMessage(hdc,"Constructing normalized patterns file...");
	if (writepatternfile(pattern,patlen,patsz,ftmp)==0)
	{
		fatal(hdc,"Unable to write to output file");
		fclose(ftmp);
	}
	else
	{
		fclose(ftmp);
		if (remove(patfname))
			//printf("\nError: unable to delete output file ('%s')\n",patfname);
            MessageBox(NULL, "Warning: unable to delete TMP file(s)", appName, MB_ICONEXCLAMATION);
		else
		{
			if (rename(TMPFILENAME,patfname))
                MessageBox(NULL, "Warning: unable to rename TMP file(s)", appName, MB_ICONEXCLAMATION);
				//printf("\nError: unable to rename output file ('%s')\n",TMPFILENAME);
			//else
			//	printf(" done.\n");
		}
	}

	//... create DAT file (full feature set) for ANN32 package ...
	DrawMessage(hdc,"Writing AUX-0 data file...");
	if ((aux=fopen(AUX0_FILENAME,"w+"))==NULL)
	{
        fatal(hdc,"Unable to create AUX-0 data file");
        return(5);
    }
	writeauxfile0(pattern,patlen,patsz,MAX_CLASS,aux);
	fclose(aux);
	//printf(" done.\n");


//... exiting to system ...
	
	//... close files & free memory buffers ...
	//printf("Cleaning up... ");
	delete pattern;
	delete imdata;  delete img;  delete box;
	fclose(pfh);
    appStatus.drawImage=TRUE;
	DrawMessage(hdc,"Process terminated normally.");

    EndPaint( hWnd, &ps );
 
    return(0);
}


//....................................


