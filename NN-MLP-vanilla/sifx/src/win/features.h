//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_FEATURES_H
#define		_FEATURES_H

#include "const.h"


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
	printf("F01 value = %g\n",res);
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
	printf("F02 value = %g\n",res);
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
        printf("Warning: divide by zero in F03\n");
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
	printf("F03 value = %g\n",res);
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
        printf("Warning: divide by zero in F04\n");
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
	printf("F04 value = %g\n",res);
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
	printf("F05 value = %g\n",res);
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
	printf("F06 value = %g\n",res);
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
        printf("Warning: divide by zero in F07\n");
#endif
        res = 0;
    }
    else
        res = (res - mx*my)/(vx*vy);

	delete px;  delete py;

#ifdef	DEBUG_PRINT
	printf("F07 value = %g\n",res);
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
	printf("F08 value = %g\n",res);
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
	printf("F09 value = %g\n",res);
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
	printf("F10 value = %g\n",res);
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
	printf("F11 value = %g\n",res);
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
	printf("F12 value = %g\n",res);
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
	printf("F13 value = %g\n",res);
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
	printf("F14 value = %g\n",res);
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
	printf("F15 value = %g\n",res);
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
	printf("F16 value = %g\n",res);
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
	printf("F17 value = %g\n",res);
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
	printf("F18 value = %g\n",res);
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
	printf("F19 value = %g\n",res);
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
	printf("F20 value = %g\n",res);
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


