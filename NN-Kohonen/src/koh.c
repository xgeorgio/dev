/*    ******************************************************************
      ***                                                            ***
      ***         Artificial Neural Networks - Kohonen model         ***
      ***         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~         ***
      ***                                                            ***
      ***           Harris Georgiou, AM:4  (csst9328@cs.uoi.gr)      ***
      ***     Dimitris Papadopoulos, AM:28 (csst9337@cs.uoi.gr)      ***
      ***                                                            ***
      ******************************************************************



        DESCRIPTION:
                        
            This file contains all source code needed to implement
          Kohonen ANNs.

            The program defines a sample networks and initializes it.
          Then, the network is trained with random input of uniform
          distribution. Training procedure associates uniform input
          with uniform output, as a "transfer" function.

            The success is dependent by various factors such as the
          parameters of learning and distance coefficients, the number
          of training cycles (repetitions) and of course the network's
          structure and the quality of random input.



------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
//#include <curses.h>
#include <string.h>

#include "kohnet.h"
#include "kohmem.c"
#include "kohnet.c"
/*#include "datanal.c"*/
#include "exitcode.h"
#include "koh.h"




/* simple continuation prompt procedure */

int prompt( char *msg )
{
    int    ch;

    printf("\n... <%s> ...",msg);
    ch = getchar();
    printf("\n");
    return(ch);
}



/*** General program flow control & termination (i.e. some exception handling ***/

void exit_to_system( char *exittype, int exitcode )
{
    printf("\nExiting to system: %s\n",exittype);
    printf("Total run-time: %g sec.\n\n",(double)(clock()/CLOCKS_PER_SEC));
    /*fcloseall();*/   /*not available!*/
    exit(exitcode);
}

void catch_cbrk( int i)
{
    exit_to_system("User break",EXIT_USERBRK);
}

void catch_fpe( int i )
{
    exit_to_system("Floating point error",EXIT_FLTPERR);
}

void catch_segv( int i )
{
    exit_to_system("Memory violation",EXIT_MEMVERR);
}




#define		usage()       printf("\nUsage:   koh  <inputfile.txt>\n")


/*******************************/


void main( int argc, char **argv )
{
    NKohNet     test;
    TCount      cycle, count;
    NCounter    cx, cy, cw;
    NCounter    centerx, centery;
    /*NData       inparr2[INPLEN];*/
    NData       inparr[SETLEN][INPLEN];
    long        outarr[HDIMX][HDIMY];
    double      avg, dev, sdev;
    FILE        *inpfile, *dumpfile;
    char        inpstr[30], dumpfilename[30];
    clock_t     start, elapsed, remaining;
    int         remh, remm, rems;

    
    signal( SIGINT, catch_cbrk );
    signal( SIGFPE, catch_fpe );
    signal( SIGSEGV, catch_segv );
    

    printf("Kohonen ANN Model Testing - version 2.1\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    /* define network structure */
    test.siz.inp = INPLEN;
    test.siz.hidx = HDIMX;
    test.siz.hidy = HDIMY;
    test.siz.outx=test.siz.hidx;
    test.siz.outy=test.siz.hidy;
    
    /* define learning and distance parameters */
    learn_T1 = LEARN_T1;            /* for a straight line, set:  T1 == ((K1-K2)/K2)*T2 */
    learn_T2 = LEARN_T2;
    dist_D1 = DIST_D1;
    dist_D2 = DIST_D2;

    if (argc<2)
    {
	printf("ERROR: You must give an input file\n");
	usage();
	exit_to_system("Cannot open input file",-1);
    }
    else if ( (argv[1][0]=='-')&&((argv[1][1]=='?')||(argv[1][1]=='h')||(argv[1][1]=='H')) )
    {
	usage();
	exit_to_system("Usage query (no action)",0);
    }
    else if ( (inpfile=fopen(argv[1],"rt")) == NULL )    
    {
 	printf("ERROR: Invalid input file name (%s)\n",argv[1]);
	usage();       
	exit_to_system("Cannot open input file",-1);
    }

    if (argv[2]!=NULL)
	strcpy(dumpfilename,argv[2]);
    else
	strcpy(dumpfilename,DEFDUMPNAME);
    if ( (dumpfile=fopen(dumpfilename,"wt")) == NULL )    
    {
        exit_to_system("Cannot open dump (output) file",-1);
    }
   
    printf("net.struct:  [%d] -> (%dx%d) -> [%dx%d]\n",
        test.siz.inp, test.siz.hidx, test.siz.hidy, test.siz.outx, test.siz.outy );

    prompt("Press any key to initialize network");    
    printf("\nInitializing Kohonen net ... ");
    printf("%d\n", nkohnet_init(&test) );

    /*init_conscience(&test);*/

    
    prompt("Press any key to start training");    
    printf("\nInitializing input and training ...\n");
    for ( count=0; count<SETLEN; count++ )
    {
        for ( cx=0; cx<INPLEN; cx++ )
        {
            fscanf(inpfile,"%s",inpstr);
            inparr[count][cx] = atof(inpstr);
        }
    }
    start = clock();
    for ( cycle=0; cycle<learn_T2; cycle++ )
    {
        if (cycle%10==0) printf("cycle=%-5ld  lcoef=%6.4f  dcoef=%d  inplen=%ld  setlen=%ld",
                    cycle+1,learn_coef(cycle),dist_coef(cycle),INPLEN,SETLEN);
        for ( count=0; count<SETLEN; count++ )
        {
            regm_update( &test, &(inparr[count][0]), cycle );
        }
        /*for ( count=0; count<INPLEN; count++ )
        {
            for ( cx=0; cx<INPLEN; cx++ )
            {
                if ( cx==count )
                    inparr2[cx] = 10;
                else
                    inparr2[cx] = 0;
            }

            regm_update( &test, inparr2, count );
        }*/

        if (cycle%SCRNINTERV==0) 
	{
	    elapsed = clock()-start;
            remaining = (clock_t)(((double)learn_T2/(cycle+1)-1)*elapsed/CLOCKS_PER_SEC);
            remh = remaining/3600;
            remaining = remaining % 3600;
            remm = remaining/60;
            remaining = remaining % 60;
            rems = remaining;
            printf("      ETA = %02d:%02d:%02d\n",remh,remm,rems);
        }
	if (cycle%DUMPINTERV==0)
	{
	    nkohnet_dump(&test,cycle,dumpfile);
	    printf("\t(Dumping network in file: %s)\n",dumpfilename);
	}

        rewind(inpfile);
    }
    
    printf("\nTraining is now complete!\n");
    prompt("Press any key to show final neuron weights");
    printf("\nFinal Results (net):\n");
    for ( cx=0; cx<(test.hid.nnumx); cx++ )
    {
        for ( cy=0; cy<(test.hid.nnumy); cy++ )
        {
            printf("    neuron [%d,%d]  =  (",cx,cy);
            for ( cw=0; cw<(test.hid.neu[cx][cy].wnum); cw++ )
            {
                printf(" %4.2f ", test.hid.neu[cx][cy].w[cw]);
            }
            printf(")\n");

        }
    }
    
    prompt("Press any key to start testing inputs");
    printf("\nTrying training inputs ...\n");
    rewind(inpfile);
    for ( cx=0; cx<HDIMX; cx++ )
        for ( cy=0; cy<HDIMY; cy++ )
            outarr[cx][cy] = 0L;
    start = clock();
    for ( count=0; count<SETLEN; count++ )
    {
        printf("inplen=%-2ld  setcount=%-4ld",INPLEN,count+1);

        nkohnet_eval( &test, &(inparr[count][0]), &centerx, &centery );
        /*printf("Max.Stim. output:  ( %d , %d )",centerx,centery);*/
        outarr[centerx][centery]++;
        
        elapsed = clock()-start;
        remaining = (clock_t)(((double)SETLEN/(count+1)-1)*elapsed/CLOCKS_PER_SEC);
        remh = remaining/3600;
        remaining = remaining % 3600;
        remm = remaining/60;
        remaining = remaining % 60;
        rems = remaining;
        printf("      ETA = %02d:%02d:%02d\n",remh,remm,rems);
    }
    /*for ( count=0; count<INPLEN; count++ )
    {
        printf("inplen=%-2ld  setcount=%-4ld",INPLEN,count+1);
        for ( cx=0; cx<INPLEN; cx++ )
        {
            if ( cx==count )
                inparr2[cx] = 10;
            else
                inparr2[cx] = 0;
        }

        nkohnet_eval( &test, inparr2, &centerx, &centery );
        printf("Max.Stim. output:  ( %d , %d )",centerx,centery);
        outarr[centerx][centery]++;
        
        elapsed = clock()-start;
        remaining = (clock_t)(((double)SETLEN/(count+1)-1)*elapsed/CLK_TCK);
        remh = remaining/3600;
        remaining = remaining % 3600;
        remm = remaining/60;
        remaining = remaining % 60;
        rems = remaining;
        printf("      ETA = %02d:%02d:%02d\n",remh,remm,rems);
    }*/

    
    printf("\nTesting is now complete!\n");
    prompt("Press any key to show final neuron stimulation counts");
    printf("\nOutputs counted:\n");
    fprintf(dumpfile,"\n\nKohonet Network - Final Results (Stimulation Counts per Neuron)\n\n");       
    for ( cx=0; cx<HDIMX; cx++ )
        for ( cy=0; cy<HDIMY; cy++ )
	{
            printf("\tneuron [ %d , %d ]  =  %ld\n",cx,cy,outarr[cx][cy]);

	    fprintf(dumpfile,"\tneuron [ %d , %d ]  =  %ld\n",cx,cy,outarr[cx][cy]);
	}
    
    fprintf(dumpfile,"\n\nProgram ended normally.\n");

    prompt("Press any key to destroy network");    
    printf("\nDestroying Kohonen net ... ");
    printf("%d\n", nkohnet_done(&test) );

    printf("\nDone!\n");

}




