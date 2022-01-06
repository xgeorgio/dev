//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
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
         //screenupdate=GetUpdateRate();
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
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0x9F,0x9F,0xCF)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0x9F,0x9F,0xCF)));

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
   SetTextColor(hdc,RGB(0xAF,0xAF,0xDF));

   sprintf(strBuffer,"%s",VERSION_TAG);
   TextOut(hdc,10,10,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Graphs");
   TextOut(hdc,220,45,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Configuration");
   TextOut(hdc,620,45,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"Results");
   TextOut(hdc,640,215,strBuffer,strlen(strBuffer));

   sprintf(strBuffer,"recognized");
   TextOut(hdc,200,205,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"%%success");
   TextOut(hdc,200,335,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"%%failure");
   TextOut(hdc,200,465,strBuffer,strlen(strBuffer));
   
   // draw graphs
   DeleteObject(pen);
   DeleteObject(brush);
   pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0xDF,0xDF,0xDF)));
   brush=SelectObject(hdc,CreateSolidBrush(RGB(0xDF,0xDF,0xDF)));
   SetTextColor(hdc,RGB(0xDF,0xDF,0xDF));

   sprintf(strBuffer,"CFG file: '%s'",appStatus.InpFileName);
   TextOut(hdc,550,65,strBuffer,strlen(strBuffer));  

   TextOut(hdc,10,510,appMesg,strlen(appMesg));

   if (appStatus.isNetwork) {
   sprintf(strBuffer,"Patterns file: '%s' (size=%d)",appStatus.patFileName,appStatus.patSize);
   TextOut(hdc,550,80,strBuffer,strlen(strBuffer));
   sprintf(strBuffer,"Feature vecs: all=%d, sel=%d, mdc=%d",appStatus.ivecSize,appStatus.svecSize,appStatus.mvecSize);
   TextOut(hdc,550,95,strBuffer,strlen(strBuffer));
  
   sprintf(strBuffer,"MDC file: '%s'",appStatus.mdcFileName);
   TextOut(hdc,550,120,strBuffer,strlen(strBuffer));

   if (appStatus.fvecsz>0) {
   sprintf(strBuffer,"Features [%d]: { ",appStatus.fvecsz);
   for ( c=0; c<appStatus.fvecsz; c++ )
       sprintf(strBuffer,"%s%d ",strBuffer,appStatus.fvec[c]);
   sprintf(strBuffer,"%s}",strBuffer);
   TextOut(hdc,550,135,strBuffer,strlen(strBuffer)); }

   sprintf(strBuffer,"T-test limit: %5.3f",appStatus.tlimit);
   TextOut(hdc,550,150,strBuffer,strlen(strBuffer));

   
   gwidth=maxleft-minleft;
   scrstep=max((int)(appStatus.maxiter/gwidth),1);
   
   sprintf(strBuffer,"Trying configuration: %d of %d",appStatus.iter,appStatus.maxiter);
   TextOut(hdc,550,235,strBuffer,strlen(strBuffer));

   if (appStatus.valPtg)  {
   MoveToEx(hdc,minleft,200,NULL);
   for ( c=0; c<appStatus.iter; c++ )
   {
       cx=(int)((maxleft-minleft)*c/appStatus.maxiter+minleft);
       cy=200-(int)appStatus.valPtg[c];  // same as recogn.pat scaled to 0-100 pts
       LineTo(hdc,cx,cy);
       MoveToEx(hdc,cx,cy,NULL);
   }
   MoveToEx(hdc,minleft,330,NULL);
   for ( c=0; c<appStatus.iter; c++ )
   {
       cx=(int)((maxleft-minleft)*c/appStatus.maxiter+minleft);
       cy=330-(int)appStatus.valPtg[c];
       LineTo(hdc,cx,cy);
       MoveToEx(hdc,cx,cy,NULL);
   }
   MoveToEx(hdc,minleft,460,NULL);
   for ( c=0; c<appStatus.iter; c++ )
   {
       cx=(int)((maxleft-minleft)*c/appStatus.maxiter+minleft);
       cy=460-(int)(100-appStatus.valPtg[c]);
       LineTo(hdc,cx,cy);
       MoveToEx(hdc,cx,cy,NULL);
   }
   }

   sprintf(strBuffer,"Best success: %d of %d  (%6.2f%%)",appStatus.tstres[0]+appStatus.tstres[3],appStatus.patSize,appStatus.bestptg);
   TextOut(hdc,550,255,strBuffer,strlen(strBuffer));
   if (appStatus.isOutput2)
   {
       pen=SelectObject(hdc,CreatePen(PS_SOLID,1,RGB(0xDF,0xDF,0xDF)));
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

       sprintf(strBuffer,"overall=%6.2f%%",appStatus.bestptg);
       TextOut(hdc,615,500,strBuffer,strlen(strBuffer));
   }

   }

   // finish drawing
   DeleteObject(pen);
   DeleteObject(brush);

   EndPaint( hWnd, &ps );
}

/*
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
*/

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

            strcpy(status->LogFileName,"");
            strcpy(status->InpFileName,"");
            strcpy(status->patFileName,"");
            strcpy(status->mdcFileName,"");

            status->ivecSize=0;  status->svecSize=0;
            status->patSize=0;  status->mvecSize=0;
            status->maxiter=0;  status->iter=0;
            if (status->valPtg)  { free(status->valPtg); status->valPtg=NULL; }

            status->tlimit=0;  status->tstptrn=0;  status->fvecsz=0;
            status->isOutput2=FALSE;
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



#ifndef     _STATLIB_H
#define		_STATLIB_H
/*
#include <math.h>

#ifndef        TRUE
#define        TRUE            1
#endif
#ifndef        FALSE
#define        FALSE           0
#endif
*/
#define        EPSILON         1.0e-15
#define        BAD_RESULT      -1.0e+30
#define        MISSING_DATA    -1.0e+30

#define        SQR(x)          (x*x)
#define        CUBE(x)         (x*x*x)
#define        ONEOVER(x)      (1/(x))
#define        RECIPROSAL(x)   (1/(x))

typedef  int  Boolean;

//enum  boolean  { false, true };
enum  answer   { no, yes, dont_care };

//const double twoPi=6.283185308;
#define        twoPi           (double)6.283185308



//..... basic distribution function .....
double Q( double x )
{
	double	result, sum=0.0, xp=1.0, tempo;
	double	b[5] = { 0.319381530, -0.356563782, 1.781477937, -1.821255978, 1.330274429 };
	int		i;

	tempo=1.0/(1.0+0.2316419*fabs(x));
	for ( i=0; i<5; i++ )
		{  xp *= tempo;  sum += b[i]*xp;  }
	result = (exp(-x*x/2.0)/sqrt(twoPi)*sum);
	result = ((x>=0.0)?1.0-result:result);

	return(result);
}

//..... inverse of basic distribution function .....
double QInv( double x )
{
	double	result, sum1=0.0, sum2=0.0, tempo, xp=1.0;
	int		i;
	double	c[4] = { 2.515517, 0.802853, 0.010328, 0.0 };
	double	d[4] = { 1.0, 1.432788, 0.189269, 0.001308 };

	
	if (x<=0.0)  x=0.0001;
	if (x>=1.0)  x=0.9999;
	
	if (x<=0.5)  tempo=sqrt(log(1.0/(x*x)));
	else  tempo=sqrt(log(1.0/(1-x*x)));

	for ( i=0; i<4; i++ )
	{
		sum1 += c[i]*xp;  sum2 += d[i]*xp;  xp *= tempo;
	}

	result = tempo-sum1/sum2;
	result = ((x>0.5)?-result:result);

	return(result);
}


//..... T-distribution function .....
double T( double x, double df )
{
	double	xt;

	xt = x*(1.0-0.25/df)/sqrt(1.0+x*x/2.0/df);

	return(1.0-Q(xt));
}

//..... T-distribution inverse function .....
double TInv( double x, double df )
{
	double	sum, xp, xq;
	double	Pwr[10], term[5];
	int		i;


	if (x<=0.0)  x=0.0001;
	if (x>=1.0)  x=0.9999;
	
	xq=QInv(x);
	Pwr[1]=xq;
	for ( i=2; i<=9; i++ )  Pwr[i]=Pwr[i-1]*xq;
	
	term[1]=(Pwr[3]+Pwr[1])/4.0;
	term[2]=(5*Pwr[5]+16*Pwr[3]+3*Pwr[1])/96.0;
	term[3]=(3*Pwr[7]+19*Pwr[5]+17*Pwr[3]-15*Pwr[1])/384.0;
	term[4]=(79*Pwr[9]+776*Pwr[7]+1482*Pwr[5]-1920*Pwr[3]-945*Pwr[1])/92160.0;

	sum=xq;
	xp=1.0;
	for ( i=1; i<=4; i++ )
	{
		xp *= df;
		sum += term[i]/xp;
	}

	return(sum);
}


//..... X-distribution function .....
double Chi( double x, double df )
{
        double  k, xq;

        k = 2.0/9.0/df;
        xq = (pow((x/df),(double)1.0/3.0)-(1.0-k))/sqrt(k);
        return(1.0-Q(xq));
}

//..... X-distribution inverse function .....
double ChiInv( double x, double df )
{
        double  k, tempo, xq;


        if (x<=0.0)  x=0.0001;
        if (x>=1.0)  x=0.9999;

        k=2.0/9.0;
        xq=QInv(x);
        tempo=1.0-k/df+xq*sqrt(k/df);
        tempo=df*CUBE(tempo);
        return(tempo);
}


//..... F-distribution function .....
double F( double x, double df1, double df2 )
{
	double	f1, f2, k;

	k = 2.0/9.0;
	f1 = pow(x,(double)1.0/3.0)*(1.0-k/df2)-(1.0-k/df1);
	f2 = sqrt(k/df1+pow(x,2.0/3.0)*k/df2);

	return(1.0-Q(f1/f2));
}

//..... F-distribution inverse function .....
double FInv( double x, double df1, double df2 )
{
        double  alpha, beta, gamma, delta, A, B, C, k, result, xq;

        if (x<=0.0)  x=0.0001;
        if (x>=1.0)  x=0.9999;

        k = 2.0/9.0;
        xq = QInv(x);
        alpha = (1-k/df2);
        beta = (1-k/df1);
        gamma = 1.0-beta;
        delta = 1.0-alpha;
        A = SQR(alpha)-SQR(xq)*delta;
        B = -2.0*alpha*beta;
        C = SQR(beta)-SQR(xq)*gamma;

        result = (-1.0*B+sqrt(SQR(B)-4.0*A*C))/(2.0*A);
        return(CUBE(result));
}


#endif     // _STATLIB_H


#ifndef		_MDC_H
#define		_MDC_H

//#include <math.h>
#include <float.h>
#include <memory.h>
//#include "const.h"

#define		MAX_CLASS		2

#define		EUC_DIST		1
#define		MAH_DIST		2
#define		DEF_DIST		MAH_DIST


typedef struct tagMDCinfo {
	int		dim;
	int		fvec[MINFEATSELECT];
	double	avg[MINFEATSELECT];
	double	var[MINFEATSELECT];	}	TMDCinfo;

typedef struct tagCPMinfo {
	long	total;
	long	count[MAX_CLASS][MAX_CLASS];
	TMDCinfo	cls[MAX_CLASS]; }	TCPMinfo;


//..........................
void MDC_init( TMDCinfo &mdc )
{
	int	fc;

	mdc.dim = 0;
	for ( fc=0; fc<mdc.dim; fc++ )
	{
		mdc.fvec[fc] = 0;
		mdc.avg[fc] = 0;
		mdc.var[fc] = 0;
	}
}

		
void MDC_copy( TMDCinfo &from, TMDCinfo &to )
{
	memcpy(&to,&from,sizeof(TMDCinfo));
}


int MDC_writetofile( TMDCinfo &mdc, FILE *mfh )
{
	int		fc;


	if (ferror(mfh))  return(-1);

	fprintf(mfh,"%d\n",mdc.dim);
	for ( fc=0; fc<mdc.dim; fc++ )
		fprintf(mfh,"%d %f %f\n",mdc.fvec[fc],mdc.avg[fc],mdc.var[fc]);
	
	return(0);
}


void MDC_printtostream( TMDCinfo &mdc, FILE *out )
{
	int		fc;

	fprintf(out,"MDCinfo:\n");
	
	fprintf(out,"\ttag = { ");
	for ( fc=0; fc<mdc.dim; fc++ ) fprintf(out,"%d ",mdc.fvec[fc]);
	fprintf(out,"}\n");
	
	fprintf(out,"\tcenter = [ ");
	for ( fc=0; fc<mdc.dim; fc++ ) fprintf(out,"%g ",mdc.avg[fc]);
	fprintf(out,"]\n");

	fprintf(out,"\tstddev = [ ");
	for ( fc=0; fc<mdc.dim; fc++ ) fprintf(out,"%g ",sqrt(mdc.var[fc]));
	fprintf(out,"]\n");
}


double MDC_distance( TMDCinfo &mdc, double *vec, int mode )
{
	int		fc;
	double	sum=0;

	switch(mode)
	{
		case MAH_DIST:
			for ( fc=0,sum=0; fc<mdc.dim; fc++ )
				sum += (mdc.avg[fc] - vec[fc])*(mdc.avg[fc] - vec[fc]) / mdc.var[fc];
			break;
		case EUC_DIST:
		default:
			for ( fc=0,sum=0; fc<mdc.dim; fc++ )
				sum += (mdc.avg[fc] - vec[fc])*(mdc.avg[fc] - vec[fc]);
			break;
	}

	return(sqrt(sum));
}

//.......................................
void CPM_init( TCPMinfo &cpm )
{
	int		fc, sc;

	cpm.total = 0;
	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		MDC_init(cpm.cls[fc]);
		for ( sc=0; sc<MAX_CLASS; sc++ )
			cpm.count[fc][sc] = 0;
	}
}

void CPM_copy( TCPMinfo &from, TCPMinfo &to )
{
	memcpy(&to,&from,sizeof(TCPMinfo));
}

int CPM_writetofile( TCPMinfo &cpm, FILE *cfh )
{
	int		fc, sc;


	if (ferror(cfh))  return(-1);

	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		fprintf(cfh,"%d\n",cpm.cls[fc].dim);
		for ( sc=0; sc<cpm.cls[fc].dim; sc++ )
			fprintf(cfh,"%d %f %f\n",cpm.cls[fc].fvec[sc],cpm.cls[fc].avg[sc],cpm.cls[fc].var[sc]);
	}

	fprintf(cfh,"%ld\n",cpm.total);
	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		for ( sc=0; sc<MAX_CLASS; sc++ )
			fprintf(cfh,"%ld\t",cpm.count[fc][sc]);
		fprintf(cfh,"\n");
	}
	
	return(0);
}


void CPM_printtostream( TCPMinfo &cpm, FILE *out )
{
	double		succ=0;

	if (cpm.total > 0)  succ=((double)cpm.count[0][0] + cpm.count[1][1])/cpm.total;

	fprintf(out,"CPMinfo:\n");
	
	fprintf(out,"\t      cN    cA  \n");
	fprintf(out,"\t    +-----------+\n");
	fprintf(out,"\tpN: | %3ld | %3ld |\n",cpm.count[0][0],cpm.count[0][1]);
	fprintf(out,"\t    +-----------+\n");
	fprintf(out,"\tpA  | %3ld | %3ld |\n",cpm.count[1][0],cpm.count[1][1]);
	fprintf(out,"\t    +-----------+\n");
	fprintf(out,"\t    succ = %5.1f%%\n",succ*100);
}


void CPM_printalltostream( TCPMinfo &cpm, FILE *out )
{
	int		fc;

	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		fprintf(out,"Class-%d configuration:\n",fc);
		MDC_printtostream(cpm.cls[fc],out);
	}
	CPM_printtostream(cpm,out);
}


int CPM_create_lvo( double *pattern, long pcount, int *select, int scount, long lvopos, TCPMinfo &cpm1 )
{
	long	pc;
	int		fc, cl, sc, ccls, clscount[MAX_CLASS];
	TCPMinfo	cpmtmp;


	CPM_init(cpmtmp);  CPM_copy(cpm1,cpmtmp);

	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		cpmtmp.cls[fc].dim = scount;
		for ( sc=0; sc<cpmtmp.cls[fc].dim; sc++ )
		{
			cpmtmp.cls[fc].fvec[sc]=select[sc];
			cpmtmp.cls[fc].avg[sc]=0;
			cpmtmp.cls[fc].var[sc]=0;
		}

		clscount[fc]=0;
	}

	//... calculate feature means ...
	for ( pc=0; pc<lvopos; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  { printf("WARNING: idx=%d\n",ccls); continue; }
		for ( fc=0; fc<scount; fc++ )
		{
			cpmtmp.cls[ccls].avg[fc] += pattern[pc*(MAX_FEATURE+1)+select[fc]];
		}
		clscount[ccls]++;
	}

	for ( pc=lvopos; pc<pcount; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  { printf("WARNING: idx=%d\n",ccls); continue; }
		for ( fc=0; fc<scount; fc++ )
		{
			cpmtmp.cls[ccls].avg[fc] += pattern[pc*(MAX_FEATURE+1)+select[fc]];
		}
		clscount[ccls]++;
	}

	for ( cl=0; cl<MAX_CLASS; cl++ )
	{
		if (clscount[cl]<=0)
        {
            printf("Warning: clscount[%d]=%d , set to 1\n",cl,clscount[cl]);
            clscount[cl]=1;
        }

		for ( fc=0; fc<cpmtmp.cls[cl].dim; fc++ )
			cpmtmp.cls[cl].avg[fc] = cpmtmp.cls[cl].avg[fc] / clscount[cl];
	}

	//... calculate feature variances ...
	for ( pc=0; pc<lvopos; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  continue;
		for ( fc=0; fc<scount; fc++ )
		{
			ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
			cpmtmp.cls[ccls].var[fc] += (cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]])*(cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]]);
		}
	}

	for ( pc=lvopos; pc<pcount; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  continue;
		for ( fc=0; fc<scount; fc++ )
		{
			ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
			cpmtmp.cls[ccls].var[fc] += (cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]])*(cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]]);
		}
	}

	for ( cl=0; cl<MAX_CLASS; cl++ )
		for ( fc=0; fc<cpmtmp.cls[cl].dim; fc++ )
			cpmtmp.cls[cl].var[fc] = fabs( cpmtmp.cls[cl].var[fc] / clscount[cl] );

	//CPM_printalltostream(cpmtmp,stdout);
	CPM_copy(cpmtmp,cpm1);

	return(0);
}


int CPM_create_all( double *pattern, long pcount, int *select, int scount, TCPMinfo &cpm1 )
{
	long	pc;
	int		fc, cl, sc, ccls, clscount[MAX_CLASS];
	TCPMinfo	cpmtmp;


	CPM_init(cpmtmp);  CPM_copy(cpm1,cpmtmp);

	for ( fc=0; fc<MAX_CLASS; fc++ )
	{
		cpmtmp.cls[fc].dim = scount;
		for ( sc=0; sc<cpmtmp.cls[fc].dim; sc++ )
		{
			cpmtmp.cls[fc].fvec[sc]=select[sc];
			cpmtmp.cls[fc].avg[sc]=0;
			cpmtmp.cls[fc].var[sc]=0;
		}

		clscount[fc]=0;
	}

	//... calculate feature means ...
	for ( pc=0; pc<pcount; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  { /*printf("WARNING: idx=%d\n",ccls);*/ continue; }
		for ( fc=0; fc<scount; fc++ )
		{
			cpmtmp.cls[ccls].avg[fc] += pattern[pc*(MAX_FEATURE+1)+select[fc]];
		}
		clscount[ccls]++;
	}

	for ( cl=0; cl<MAX_CLASS; cl++ )
	{
		if (clscount[cl]<=0)
        {
            //printf("Warning: clscount[%d]=%d , set to 1\n",cl,clscount[cl]);
            clscount[cl]=1;
        }

		for ( fc=0; fc<cpmtmp.cls[cl].dim; fc++ )
			cpmtmp.cls[cl].avg[fc] = cpmtmp.cls[cl].avg[fc] / clscount[cl];
	}

	//... calculate feature variances ...
	for ( pc=0; pc<pcount; pc++ )
	{
		ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		if (ccls>MAX_CLASS-1)  continue;
		for ( fc=0; fc<scount; fc++ )
		{
			ccls=(int)pattern[pc*(MAX_FEATURE+1)+0];
			cpmtmp.cls[ccls].var[fc] += (cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]])*(cpmtmp.cls[ccls].avg[fc] - pattern[pc*(MAX_FEATURE+1)+select[fc]]);
		}
	}

	for ( cl=0; cl<MAX_CLASS; cl++ )
		for ( fc=0; fc<cpmtmp.cls[cl].dim; fc++ )
			cpmtmp.cls[cl].var[fc] = fabs( cpmtmp.cls[cl].var[fc] / clscount[cl] );

	//CPM_printalltostream(cpmtmp,stdout);
	CPM_copy(cpmtmp,cpm1);

	return(0);
}


int CPM_selectclass( TCPMinfo &cpm, double *vec, int mode )
{
	int		cl, mincls;
	double	dist[MAX_CLASS], mindist=DBL_MAX;

	for ( cl=0; cl<MAX_CLASS; cl++ )
	{
		dist[cl] = MDC_distance(cpm.cls[cl],vec,mode);
		if (dist[cl]<mindist)
			{ mindist=dist[cl]; mincls=cl; }
	}

	//printf("min.Dist = %g , class=%d\n",mindist,mincls);

	return(mincls);
}


double CPM_create( double *pattern, long pcount, int *select, int scount, int mode, TCPMinfo &cpm )
{
	long		lvo, c;
	TCPMinfo	cpmtmp;
	double		succ, vec[MINFEATSELECT];
	int			sc, corrcls, selcls;
	

	CPM_init(cpmtmp);

	for ( lvo=0; lvo<pcount; lvo++ )
	{
		CPM_create_lvo(pattern,pcount,select,scount,lvo,cpmtmp);
		//CPM_printalltostream(cpmtmp,stdout);
		
		for ( sc=0; sc<MINFEATSELECT; sc++ )
			vec[sc]=pattern[lvo*(MAX_FEATURE+1)+select[sc]];
		
		corrcls=(int)pattern[lvo*(MAX_FEATURE+1)+0];
		selcls=CPM_selectclass(cpmtmp,vec,mode);
		cpmtmp.count[corrcls][selcls]++;
		cpmtmp.total++;
	}

	for ( c=0,succ=0; c<MAX_CLASS; c++ )
		succ += (double)cpmtmp.count[c][c];
	succ = succ / cpmtmp.total;

	CPM_copy(cpmtmp,cpm);

	return(succ);
}


double CPM_optimum( HDC hdc, double *pattern, long pcount, /*int **select,*/ long slen, int *FeatConfig, int mode, TCPMinfo &cpm )
{
	long		csel, bestsel=0, pc;
	int			fsel[MINFEATSELECT], flen, corrcls, selcls, c, fc;
	TCPMinfo	cpmtmp, cpmbest, cpm1;
	double		succ, bestsucc=0, fvec[MINFEATSELECT];
    char        msg[256];


	//printf("\nOPTIMIZING MDC CONFIGURATION:\n");
    DrawMessage(hdc,"Optimizing MDC configuration...");
	CPM_init(cpmtmp); CPM_init(cpmbest);

    appStatus.maxiter=slen;
    if ((appStatus.valPtg=(float*)calloc(appStatus.maxiter,sizeof(float)))==NULL)
        fatal(hdc,"Not enough memory for graph data");

	for ( csel=0; csel<slen; csel++ )
	{
        appStatus.iter=csel;

		for ( fc=0; fc<MINFEATSELECT; fc++ )
			fsel[fc]=FeatConfig[csel*(MINFEATSELECT+1)+fc+1];
		flen=FeatConfig[csel*(MINFEATSELECT+1)+0];

		succ=CPM_create(pattern,pcount,fsel,flen,mode,cpmtmp);
        appStatus.valPtg[appStatus.iter++]=(float)(bestsucc*100.0);

        if (succ>bestsucc)
		{
            appStatus.isNetwork=TRUE;

			bestsucc=succ;  bestsel=csel;
			CPM_copy(cpmtmp,cpmbest);
			//printf("CHANGED (config=%ld): Correct = %7.3f%%\n",csel+1,bestsucc*100);
			//CPM_printalltostream(cpmbest,stdout);
            appStatus.tstres[0]=cpmbest.count[0][0];
            appStatus.tstres[1]=cpmbest.count[0][1];
            appStatus.tstres[2]=cpmbest.count[1][0];
            appStatus.tstres[3]=cpmbest.count[1][1];
            appStatus.tstptrn=cpmbest.total;
            appStatus.bestptg=(float)(succ*100.0);
		}

	    for ( fc=0; fc<FeatConfig[bestsel*(MINFEATSELECT+1)+0]; fc++ )
    		appStatus.fvec[fc]=FeatConfig[bestsel*(MINFEATSELECT+1)+fc+1];
    	appStatus.fvecsz=FeatConfig[bestsel*(MINFEATSELECT+1)+0];

        sprintf(msg,"Current optimum MDC (config=%ld): Correct = %7.3f%%",csel+1,bestsucc*100);
        DrawMessage(hdc,msg);
	}

	//printf("\nBEST CONFIGURATION:\n");
	//CPM_printalltostream(cpmbest,stdout);

	//... create (full) optimum classifier ...
	//printf("\nOPTIMUM FEATURE SELECTION:  { ");
	for ( fc=0; fc<FeatConfig[bestsel*(MINFEATSELECT+1)+0]; fc++ )
	{
		fsel[fc]=FeatConfig[bestsel*(MINFEATSELECT+1)+fc+1];
		//printf("%d ",fsel[fc]);
	}
	//printf("}\n");
	flen=FeatConfig[bestsel*(MINFEATSELECT+1)+0];

	CPM_init(cpm1);
	CPM_create_all(pattern,pcount,fsel,flen,cpm1);

	for ( pc=0; pc<pcount; pc++ )
	{
		for ( fc=0; fc<MINFEATSELECT; fc++ )
			fvec[fc]=pattern[pc*(MAX_FEATURE+1)+fsel[fc]];

		corrcls=(int)pattern[pc*(MAX_FEATURE+1)+0];
		selcls=CPM_selectclass(cpm1,fvec,mode);
		cpm1.count[corrcls][selcls]++;
		cpm1.total++;
	}

	for ( c=0,succ=0; c<MAX_CLASS; c++ )
		succ += (double)cpm1.count[c][c];
	succ = succ / cpm1.total;

	//printf("\nOPTIMUM MDC:\n");
	//CPM_printalltostream(cpm1,stdout);
    sprintf(msg,"Finished: Optimum MDC (config=%ld): Correct = %7.3f%%",bestsel+1,bestsucc*100);
    DrawMessage(hdc,msg);

	CPM_copy(cpm1,cpm);
	return(succ);
}



#endif



//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


//#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <time.h>
//#include "const.h"
//#include "statlib.h"
//#include "mdc.h"


typedef struct tagPoint {
	int		x, y;		} TPoint;

typedef struct tagPtvalue {
	int		pt;
	double	val;		} TPtvalue;


//.....................................

//... augmented output file for use with ANN32 package, selected (T-test) feature set ...
long writeauxfile1( double *pattern, long patlen, long patsize, int clscount, int *fset, int fsize, FILE *fto )
{
	long	pc, fc;
	double	cls;

	for ( pc=0; pc<patlen; pc++ )
	{
		cls = *(pattern+0+pc*patsize);
		for ( fc=0; fc<fsize; fc++ )
        {
            // check: 'fset' is treated as flags vector (AUX1)
			if (fset[fc])  fprintf(fto,"%8.3f\t",*(pattern+(fc+1)+pc*patsize));
        }
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

//... augmented output file for use with ANN32 package, optimum (MDC) feature set ...
long writeauxfile2( double *pattern, long patlen, long patsize, int clscount, int *fset, int fsize, FILE *fto )
{
	long	pc, fc;
	double	cls;

	for ( pc=0; pc<patlen; pc++ )
	{
		cls = *(pattern+0+pc*patsize);
		for ( fc=0; fc<fsize; fc++ )
        {
            // check: 'fset' is treated as indexes vector (AUX2)
			fprintf(fto,"%8.3f\t",*(pattern+fset[fc]+pc*patsize));
        }
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
double * readpatterns( FILE *ffrom, long *patlen, long *patsize )
{
	long	count=0, pc=0, nrm_fc=0, abn_fc=0;
	char    sval[MAX_LINESIZE];
	int		t, currf=0, fn, ok=1;
	double	dval, *pattern;
    char    msg[256];


	rewind(ffrom);
    fscanf(ffrom,"%ld%d",&count,&fn);
	
	if ((count<=0)||(fn<=0))
	{
		//printf("\nError: invalid patterns file header\n");
		return(0);
	}

	*patlen=count;  *patsize=fn;

	pattern=new double[count*fn];
	if (!pattern)
	{
		//printf("\nError: unable to allocate patterns buffer\n");
		if (pattern)  delete pattern;
		return(0);
	}

	for ( pc=0; pc<count; pc++ )
		for ( t=0; t<fn; t++ )
		{
			fscanf(ffrom,"%s",sval);  dval=strtod(sval,NULL);
			if ((dval==HUGE_VAL)||(dval==-HUGE_VAL)||(_isnan(dval)))
            {
				sprintf(msg,"Error: pattern[%ld][%d] = NaN  (%g)",pc,t,dval);
                MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
            }
			pattern[pc*(MAX_FEATURE+1)+t]=dval;
		}

	//sprintf(msg,"patterns read:  count=%ld , features=%d\n",pc,t-1);
    //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);

	if (ok)  return(pattern);
	else  { delete pattern; return(NULL); }
}

//... feature-Tvalue comparison function (used by qsort()) ...
int Ptvalue_compare(const void *elem1, const void *elem2)
{
	TPtvalue		*pv1=(TPtvalue*)elem1, *pv2=(TPtvalue*)elem2;

	if (pv1->val > pv2->val)  return(-1);
	else if (pv1->val < pv2->val)  return(1);
	else  return(0);
}

//... perform T-Test in pattern features ...
long selectfeatures( HDC hdc, double critical, int leastsel, double *tfunc, int *selection, TPtvalue *fvec, double *pattern, long patlen,long patsz )
{
	long	pcount, nrm_c=0, abn_c=0;
	int		fcount, selected=0;
	double	nrm_m=0, nrm_v=0, abn_m=0, abn_v=0;
	double	sval, tval, qval;
    char    msg[256];


    //MessageBox(NULL, "Starting selectfeatures", appName, MB_ICONEXCLAMATION);

    *tfunc = TInv(critical,(double)patlen);

	for ( fcount=1; fcount<patsz; fcount++ )
	{
		nrm_m=0; nrm_v=0; abn_m=0; abn_v=0;
		sval=0; tval=0; qval=0;
		nrm_c=0; abn_c=0;

		for ( pcount=0; pcount<patlen; pcount++ )
		{
#ifdef	DEBUG_PRINT
			//printf("pattern %ld:  class=%g\n",pcount+1,pattern[pcount*(MAX_FEATURE+1)+0]);
#endif

			if (pattern[pcount*(MAX_FEATURE+1)+0]==0.0)  { nrm_m = nrm_m + pattern[pcount*(MAX_FEATURE+1)+fcount]; nrm_c++; }
			else if (pattern[pcount*(MAX_FEATURE+1)+0]==1.0)  { abn_m = abn_m + pattern[pcount*(MAX_FEATURE+1)+fcount]; abn_c++; }
			else  //continue;  //-> unknown pattern class
            {
                sprintf(msg,"Invalid pattern class (=%g)",pattern[pcount*(MAX_FEATURE+1)+0]);
                MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
            }
		}
		nrm_m = nrm_m / nrm_c;
		abn_m = abn_m / abn_c;
        //sprintf(msg,"nrm_c=%d , abn_c=%d",nrm_c,abn_c);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
        //sprintf(msg,"nrm_m=%f , abn_m=%f",nrm_m,abn_m);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
		

		for ( pcount=0; pcount<patlen; pcount++ )
		{
			if (pattern[pcount*(MAX_FEATURE+1)+0]==0.0)
				{ nrm_v = nrm_v + (pattern[pcount*(MAX_FEATURE+1)+fcount]-nrm_m)*(pattern[pcount*(MAX_FEATURE+1)+fcount]-nrm_m); }
			else if (pattern[pcount*(MAX_FEATURE+1)+0]==1.0)
				{ abn_v = abn_v + (pattern[pcount*(MAX_FEATURE+1)+fcount]-abn_m)*(pattern[pcount*(MAX_FEATURE+1)+fcount]-abn_m); }
			else  continue;  //-> unknown pattern class
		}
        //sprintf(msg,"nrm_c=%d , abn_c=%d",nrm_c,abn_c);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
		
		nrm_v = sqrt( nrm_v/nrm_c );
		abn_v = sqrt( abn_v/abn_c );
        //sprintf(msg,"nrm_v=%f , abn_v=%f",nrm_v,abn_v);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);

#ifdef	DEBUG_PRINT
		{
			//printf("Feature %02d:\n",fcount);
			//printf("\t  Normal:  count=%ld , mean=%g , var=%g\n",nrm_c,nrm_m,nrm_v);
			//printf("\tAbnormal:  count=%ld , mean=%g , var=%g\n",abn_c,abn_m,abn_v);
		}
#endif

		sval = ( nrm_v*nrm_v*(nrm_c-1) + abn_v*abn_v*(abn_c-1) ) / ( nrm_c + abn_c - 2 );
		sval = sqrt( sval );

        //sprintf(msg,"nrm_c=%d , abn_c=%d",nrm_c,abn_c);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);
		
        qval = nrm_c*abn_c/(nrm_c+abn_c);

		if ((sval==0)||(qval<0))  tval=0.0;
		else
			tval = ( fabs(nrm_m-abn_m)/sval ) * sqrt( nrm_c*abn_c/(nrm_c+abn_c) );
		
        //sprintf(msg,"tval=%f , tfunc=%f",tval,*tfunc);
        //MessageBox(NULL, msg, appName, MB_ICONEXCLAMATION);

        if (tval > *tfunc)
			{ selection[fcount-1]=1;  selected++; }
		else  selection[fcount-1]=0;

#ifdef	DEBUG_PRINT
		//printf("feature %-2d  ->  T-Test: tval=%6.4f (sval=%6.4f,Fdeg=%ld)\t-> %s\n",fcount,tval,sval,patlen-2,((selection[fcount-1])?"+[Selected]":" [Rejected]"));
#endif

		fvec[fcount-1].pt=fcount-1;  fvec[fcount-1].val=tval;
	}

	qsort(fvec,patsz-1,sizeof(TPtvalue),Ptvalue_compare);

	if (selected<leastsel)
	{
		for ( fcount=selected; fcount<leastsel; fcount++ )  selection[fvec[fcount].pt]=1;
		selected=leastsel;
	}

	DrawMessage(hdc,"Sorted feature selection:");
	for ( fcount=0; fcount<patsz-1; fcount++ )
    {
		sprintf(msg,"feature %02d -> tval=%6.4f\t-> %s",fvec[fcount].pt+1,fvec[fcount].val,((fcount<selected)?"+[Selected]":" [Rejected]"));
        DrawMessage(hdc,msg);
    }

	return(selected);
}

//.......................................

//... create feature combinations (2,3,4) ...
long createfeaturecombs4( int max_features, int *FeatComb, long fcombs, long *FeatCombN )
{
	long	ncombs=0;
	int		i, j, k, m;


	//for ( i=0; i<FEATURE_COMBS; i++ )
	for ( i=0; i<fcombs; i++ )
		for ( j=0; j<MAX_FEATURE; j++ )
			FeatComb[j+MAX_FEATURE*i] = 0;

	for ( i=0; i<MAX_FEATURE; i++ )  FeatCombN[i] = 0;


#ifdef	DEBUG_PRINT
	//printf("Feature combinations-2:\n");
#endif
	for ( i=0; i<max_features-1; i++ )
		for ( j=i+1; j<max_features; j++ )
		{
			FeatComb[ncombs*MAX_FEATURE+i] = 1;
			FeatComb[ncombs*MAX_FEATURE+j] = 1;
#ifdef	DEBUG_PRINT
			//printf("\t%d\t%d\n",i+1,j+1);
#endif
			ncombs++;  FeatCombN[2]++;
		}

#ifdef	DEBUG_PRINT
	//printf("Feature combinations-3:\n");
#endif
	for ( i=0; i<max_features-2; i++ )
		for ( j=i+1; j<max_features-1; j++ )
			for ( k=j+1; k<max_features; k++ )
			{
				FeatComb[ncombs*MAX_FEATURE+i] = 1;
				FeatComb[ncombs*MAX_FEATURE+j] = 1;
				FeatComb[ncombs*MAX_FEATURE+k] = 1;
#ifdef	DEBUG_PRINT
				//printf("\t%d\t%d\t%d\n",i+1,j+1,k+1);
#endif
				ncombs++;  FeatCombN[3]++;
			}

#ifdef	DEBUG_PRINT
	//printf("Feature combinations-4:\n");
#endif
	for ( i=0; i<max_features-3; i++ )
		for ( j=i+1; j<max_features-2; j++ )
			for ( k=j+1; k<max_features-1; k++ )
				for ( m=k+1; m<max_features; m++ )
				{
					FeatComb[ncombs*MAX_FEATURE+i] = 1;
					FeatComb[ncombs*MAX_FEATURE+j] = 1;
					FeatComb[ncombs*MAX_FEATURE+k] = 1;
					FeatComb[ncombs*MAX_FEATURE+m] = 1;
#ifdef	DEBUG_PRINT
					//printf("\t%d\t%d\t%d\t%d\n",i+1,j+1,k+1,m+1);
#endif
					ncombs++;  FeatCombN[4]++;
				}

//#ifdef	DEBUG_PRINT
	//printf("Total combinations:  %ld  (%ld+%ld+%ld)\n",ncombs,FeatCombN[2],FeatCombN[3],FeatCombN[4]);
//#endif

	return(ncombs);
}


int mapfeatures( int *sel_features, int *SelFeatMap, int maxfeat )
{
	int		i, j;


	for ( i=0; i<maxfeat; i++ )  SelFeatMap[i]=0;

	for ( i=0,j=0; i<maxfeat; i++ )
		if (sel_features[i])  SelFeatMap[j++]=i+1;

#ifdef	DEBUG_PRINT
	{
		//printf("Feature mapping:\n");
		for ( i=0; i<j; i++ )  printf("\t[%d] = F%d\n",i,SelFeatMap[i]);
	}
#endif

	return(j);
}

void translatefeatcombin4( long combs, int *FeatComb, int *SelFeatMap, int maxfeat )
{
	long	cc;
	int		cf, j, sel;

	for ( cc=0; cc<combs; cc++ )
		for ( cf=0,j=0; cf<maxfeat; cf++ )
		{
			sel=FeatComb[cc*MAX_FEATURE+cf];  FeatComb[cc*MAX_FEATURE+cf]=0;
			if (sel)  { FeatComb[cc*MAX_FEATURE+j]=SelFeatMap[cf]; j++; }
		}
}

//... write current feature selection patterns ...
long writeselcombins( char *preffix, long combs, int *FeatComb, double *pattern, long patlen )
{
	long	cc, cp;
	char	fname[MAX_LINESIZE];
	FILE	*fout;
	int		cf, sn[4], fts;


	for ( cc=0; cc<combs; cc++ )
	{
		for ( cf=0,fts=0; cf<4; cf++ )
		{
			sn[cf]=FeatComb[cc*MAX_FEATURE+cf];
			if (sn[cf])  fts++;
		}

		sprintf(fname,"%s%02d%02d%02d%02d.txt",preffix,sn[0],sn[1],sn[2],sn[3]);
#ifdef	DEBUG_PRINT
		//printf("Saving: '%s' (#feat=%d)\n",fname,fts);
#endif

		if ((fout=fopen(fname,"w+"))==NULL)
		{ 
			//printf("Error: unable to create output file ('%s')\n",fname);
			return(cc);
		}

		fprintf(fout,"%ld %d\n",patlen,fts+1);
		
		for ( cp=0; cp<patlen; cp++ )
		{
			fprintf(fout,"%5.3f\t",pattern[cp*(MAX_FEATURE+1)+0]);
			for ( cf=0; cf<4; cf++ )
				if (sn[cf])  fprintf(fout,"%6.4f\t",pattern[cp*(MAX_FEATURE+1)+sn[cf]]);
			fprintf(fout,"\n");
		}

		fclose(fout);
	}

	return(cc);
}


//... store current feature selection patterns ...
long storeselcombins( long combs, int *FeatComb, long fcombs, int *FeatConfig )
{
	long	cc;
	int		cf;


	//for ( cc=0; cc<FEATURE_COMBS; cc++ )
	for ( cc=0; cc<fcombs; cc++ )
		for ( cf=0; cf<MINFEATSELECT+1; cf++ )
			FeatConfig[cc*(MINFEATSELECT+1)+cf]=0;

	for ( cc=0; cc<combs; cc++ )
	{
		for ( cf=0; cf<MINFEATSELECT; cf++ )
		{
			FeatConfig[cc*(MINFEATSELECT+1)+cf+1]=FeatComb[cc*MAX_FEATURE+cf];
			if (FeatConfig[cc*(MINFEATSELECT+1)+cf+1])  FeatConfig[cc*(MINFEATSELECT+1)+0]++;
		}

#ifdef	DEBUG_PRINT
		{
			//printf("[%d]: ",FeatConfig[cc*(MINFEATSELECT+1)+0]);
			//for ( cf=0; cf<MINFEATSELECT; cf++ )  printf("%d ",FeatConfig[cc*(MINFEATSELECT+1)+cf+1]);
			//printf("\n");
		}
#endif
	}

	return(cc);
}


//-------------------------------------------

int console_main( HWND hWnd )
{
	char	patfname[30], clsfname[30];
	float	ttestlim;
	double	flimit;
	FILE	*pfh=NULL, *cfh=NULL, *aux=NULL;

	int		fselect[MAX_FEATURE], SelFeatMap[MAX_FEATURE], fselcount=0, i, minsel=0;
	long	FeatCombN[MAX_FEATURE];
	int		*FeatComb=NULL;
	int		*FeatConfig=NULL;
	double	*pattern=NULL;

	int		mode=DEF_DIST, inpmode;
	long	tcombs=0, fcombs=0, wcombs=0, scount=0, sc=0, patlen=0, patsz=0;
	TPtvalue	fvec[MAX_FEATURE];
	TCPMinfo	cpm;
    char    msg[256];
    PAINTSTRUCT ps;
    HDC hdc;


    // initialize drawing
    hdc = BeginPaint( hWnd, &ps );
      
    //printf("FCD: Feature Classifier Design, v%s (%s) - Harris Georgiou (c) 2000\n",VERSION_VER,VERSION_REL);
    //printf("-------------------------------------------------------------------------\n\n");
    

//... phase 0: initialization ...

	//printf("Give output patterns filename: ");
    fscanf(appStatus.inpfile,"%s",patfname);
	if ((pfh=fopen(patfname,"r+"))==NULL)
	{ 
		fatal(hdc,"Unable to open patterns file");
		//return(3);
	}
    strcpy(appStatus.patFileName,patfname);


//... phase 1: construct optimized patterns file (selected feature set) ...
	
	//printf("\nGive T-Test critical value (0.0-1.0): ");
    fscanf(appStatus.inpfile,"%f",&ttestlim);
	if (ttestlim<0)  ttestlim=-ttestlim;
	if (ttestlim>1)  ttestlim=1;
    appStatus.tlimit=ttestlim;

	//printf("Give minimum feature selection count (0 for default): ");
	fscanf(appStatus.inpfile,"%d",&minsel);
	if (minsel<ATLEASTSELECT)  minsel=ATLEASTSELECT;
    appStatus.minsel=minsel;

	DrawMessage(hdc,"Optimizing feature selection...");
	if ((pattern=readpatterns(pfh,&patlen,&patsz))==0)
        fatal(hdc,"Unable to read patterns file");
	else
	{
		//printf(" done.\n");
        appStatus.patSize=patlen;
        appStatus.ivecSize=patsz;

		fselcount = selectfeatures(hdc,ttestlim,minsel,&flimit,fselect,fvec,pattern,patlen,patsz);
        appStatus.svecSize=fselcount;
		sprintf(msg,"T-Test Results:  selected %d out of %d features (limit=%5.3f)",fselcount,MAX_FEATURE,flimit);
        DrawMessage(hdc,msg);
	}

    appStatus.isNetwork=TRUE;

	sprintf(msg,"Selected feature set: [%d]: ",fselcount);
	for ( i=0; i<MAX_FEATURE; i++ )  sprintf(msg,"%s%d ",msg,fselect[i]);
	//printf("\n");
    DrawMessage(hdc,msg);
    sprintf(msg,"Selected features (t>=%5.3f): %d",ttestlim,fselcount);
    MessageBox(NULL, msg, appName, MB_ICONINFORMATION);

  //... create DAT file (selected feature set) for ANN32 package ...
	DrawMessage(hdc,"Writing AUX-1 data file...");
	if ((aux=fopen(AUX1_FILENAME,"w+"))==NULL)
		{ fatal(hdc,"Error: unable to create AUX-1 data file\n"); return(5);	}
	writeauxfile1(pattern,patlen,patsz,MAX_CLASS,fselect,MAX_FEATURE,aux);
	fclose(aux);
	//printf(" done.\n");


//... phase-2: create feature combinations (2,3,4) ...
	tcombs=ncombs(fselcount);
	FeatComb=new int[tcombs*MAX_FEATURE];
	FeatConfig=new int[tcombs*(MINFEATSELECT+1)];
	if ((!FeatComb)||(!FeatConfig))
	{
		fatal(hdc,"Error: not enough memory for feature combinations");
		if (FeatComb)  delete FeatComb;
		if (FeatConfig)  delete FeatConfig;
		fclose(pfh);
	}
	else
    {
		sprintf(msg,"Allocated space for %ld feature combinations",tcombs);
        DrawMessage(hdc,msg);
    }

	fcombs=createfeaturecombs4(fselcount,FeatComb,tcombs,FeatCombN);
	mapfeatures(fselect,SelFeatMap,MAX_FEATURE);

	//printf("translating feature combinations\n");
	translatefeatcombin4(fcombs,FeatComb,SelFeatMap,MAX_FEATURE);

	//printf("saving feature combinations... ");
	//wcombs=writeselcombins(PATFILE_PREFX,fcombs,patlen);
	wcombs=storeselcombins(fcombs,FeatComb,tcombs,FeatConfig);
	/*if (wcombs==fcombs)  printf("done (%ld configurations saved)\n",wcombs);
	else  printf("Error: (I/O) %ld of %ld configurations saved\n",wcombs,fcombs);*/


//... phase-3: min.dist.classifier design ...

	//printf("Give distance mode (%d=Eucl,%d=Mahl): ",EUC_DIST,MAH_DIST);
    fscanf(appStatus.inpfile,"%d",&inpmode);
	switch(inpmode)
	{
		case EUC_DIST:
		case MAH_DIST: mode=inpmode; break;
		default:  DrawMessage(hdc,"Warning: using default mode"); break;
	}
	switch(inpmode)
	{
		case EUC_DIST: strcpy(appStatus.distmode,"EUC"); break;
		case MAH_DIST: 
		default:  strcpy(appStatus.distmode,"MAH"); break;
	}
	
	//printf("Give MDC output filename: ");
    fscanf(appStatus.inpfile,"%s",clsfname);
	if ((cfh=fopen(clsfname,"w+"))==NULL)
	{ 
		fatal(hdc,"Unable to create MDC file");
		//return(5);
    }
    strcpy(appStatus.mdcFileName,clsfname);
		
    //MessageBox(NULL, "Starting CPM optimization", appName, MB_ICONINFORMATION);
    appStatus.isOutput2=TRUE;
	CPM_init(cpm);
	CPM_optimum(hdc,pattern,patlen,fcombs,FeatConfig,mode,cpm);
    MessageBox(NULL, "CPM optimization completed", appName, MB_ICONINFORMATION);

	DrawMessage(hdc,"Writing optimum MDC configuration to file");
	if (CPM_writetofile(cpm,cfh)!=0)  //printf("done.\n");
	//else
	{ 
		fatal(hdc,"Unable to write to MDC file");
		//return(5);
	}

	fclose(cfh);
		
  //... create DAT file (MDC-optimum feature set) for ANN32 package ...
	DrawMessage(hdc,"Writing AUX-2 data file");
	if ((aux=fopen(AUX2_FILENAME,"w+"))==NULL)
		{ fatal(hdc,"Unable to create AUX-2 data file"); return(5);	}
	writeauxfile2(pattern,patlen,patsz,MAX_CLASS,cpm.cls[0].fvec,cpm.cls[0].dim,aux);
	fclose(aux);
	//printf(" done.\n");

//... exiting to system ...
	//printf("Cleaning up... ");
	if (pattern) delete pattern;
    if (FeatComb) delete FeatComb;
    if (FeatConfig) delete FeatConfig;
	DrawMessage(hdc,"Process terminated normally.");

    EndPaint( hWnd, &ps );
 
    return(0);
}

