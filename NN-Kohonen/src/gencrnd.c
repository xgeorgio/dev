#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void main( int argc, char **argv )
{
    double      rndval=0.0, /*sum=0.0,*/ rndcoef=0.0;
    long        gcount=0, glimit=0, llimit=1L;


	if (argc<3)
	{
		printf("Usage:  %s <size> <coef>\n\t(coef = 0...1)\n",argv[0]);
		return;
	}
	
    glimit = atol(argv[1])*llimit;
    rndcoef = atof(argv[2]);
    
    if ((rndcoef>10)||(rndcoef<-10))
        rndcoef = 10.0;
    if (rndcoef == 0.0)
        rndcoef = 1.0;

    /*randomize();*/
    for ( gcount=1; gcount<=glimit; gcount++ )
    {
        rndval = rndcoef*((double)rand()/RAND_MAX);
        /*sum = sum + rndval;*/
        printf("%5.3f ",rndval);
        printf("%5.3f\n",sqrt(1-rndval*rndval));
        
	/*randomize();*/
    }
    /*printf("\nMean value: %f\n",sum/glimit);*/
}

