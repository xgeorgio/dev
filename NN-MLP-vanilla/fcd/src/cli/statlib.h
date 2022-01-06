//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef         _STATLIB_H
#define		_STATLIB_H

#include <math.h>

#ifndef        TRUE
#define        TRUE            1
#endif
#ifndef        FALSE
#define        FALSE           0
#endif
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
