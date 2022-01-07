#include <stdio.h>
#include <stdlib.h>


void main( int argc, char **argv )
{
    double      rndval=0.0, /*sum=0.0,*/ rndcoef=0.0;
    long        gcount=0, glimit=0, llimit=1;


	if (argc<4)
	{
		printf("Usage:  %s <cols> <rows> <scale>\n",argv[0]);
		return;
	}
	
    llimit = atol(argv[2]);
    glimit = atol(argv[1])*llimit;
    rndcoef = atof(argv[3]);
    
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
        if ((gcount % llimit) == 0)
        {
            printf("\n");
            /*randomize();*/
        }
    }
    /*printf("\nMean value: %f\n",sum/glimit);*/
}

