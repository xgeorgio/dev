//----------------------------------------------------
//  
//   File: Multi-Layer Perceptron ANN executor, v1.7
//
//----------------------------------------------------
//                Harris Georgiou, 2000.
//----------------------------------------------------


#ifndef __MLP_IMP2_H__
#define __MLP_IMP2_H__

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mlp_cnst.h"



/*******************************************************************************************/

void __inline fatal(const char *mes)
{
    printf("%s", mes);
    exit(1);
}

/*******************************************************************************************/

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

/*******************************************************************************************/
                                                                                
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
                                        
/*******************************************************************************************/

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


/*******************************************************************************************/

void create_net( TNet *BP_net )
{
    TCount      nlay=0;
    TCount      nneu=0;
    TCount      nweights, nwei=0;
    TWeight     /*weight[MAXWEIGHTNUM],*/ *weight;
    int         res=0;
                    
    res = init_net( BP_net, nlayers );
    if (res==0)
    {
	//printf("Memory for net allocated correctly!\n");
	
	nweights=ivecsz;

	for ( nlay=0; nlay<nlayers; nlay++ )
	{
	    res = init_layer( &(BP_net->lay[nlay]), neurons[nlay] );
	    if (res==0)
	    {
		//printf("Memory allocated for layer #%d.\n", nlay+1);
		for( nneu=0; nneu<neurons[nlay]; nneu++ )
	    	{	
			if ((weight=(TWeight*)calloc(nweights+1,sizeof(TWeight)))==NULL)
				fatal("Error: cannot allocate temp memory for weights");

		    for( nwei=0; nwei<nweights+1; nwei++ )
		 	    weight[nwei]= (TWeight)(rand() / (TWeight)RAND_MAX - 0.5);
		    res = init_neuron( &(BP_net->lay[nlay].neu[nneu]), nweights, weight, weight[nweights]);
		    if(res)
		    {
			printf("ERROR: Cannot allocate memory for neuron #%d in layer #%d\n", nneu, nlay);
		     	exit(1);
		    }
			free(weight);
	        }

		nweights = neurons[nlay];   
	    }
	    else
	    {	
		printf("ERROR: Cannot allocate memory for layer %d\n",nlay);
		exit(1);
	    }
	}
		
    }
    else
       fatal("ERROR: Cannot allocate memory for layers\n");
}

/*******************************************************************************************/
                                        
void __inline done_neuron( TNeuron *neu )
{
    free(neu->w);
    free(neu->wprev);
}

/*******************************************************************************************/
    
void __inline done_layer( TLayer *lay )
{
    TCount      nneurons;
    for ( nneurons=0; nneurons<(lay->nnum); nneurons++ )
        done_neuron( &(lay->neu[nneurons]) );
    free(lay->neu);
}

/*******************************************************************************************/
                    
void __inline ddone_net( TNet *net )
{
    TCount      nlayers;
    for ( nlayers=0; nlayers<(net->lnum); nlayers++ )
        done_layer( &((net->lay)[nlayers]) );
    free(net->lay);
}

/*******************************************************************************************/

void __inline done_net(TNet *bp)
{
    ddone_net(bp);
}

/************************************************************************/


/*=============== load network topologies & weights for further use =============*/
/*... read network configuration from file ...*/
long quickload_net( FILE *cfgfile, TNet *BP_net )
{
	int    res=0;
    TCount nlay, nneu, nwei;

	/*... read net topology from file header ...*/
	// # of layers
	//res += fread(&nlayers,sizeof(TCount),1,cfgfile);
	res += fscanf(cfgfile,"%d",&nlayers);

	// # of input vector dimension
	//res += fread(&ivecsz,sizeof(TCount),1,cfgfile);
	res += fscanf(cfgfile,"%d",&ivecsz);

	if ((neurons=(TCount*)calloc(nlayers,sizeof(TCount)))==NULL)
		fatal("Error: cannot allocate temp memory for neurons");

	// # of neuron weight vector dimensions
	//res += fread(&(neurons[0]),sizeof(TCount),nlayers,cfgfile);
	for ( nneu=0; nneu<nlayers; nneu++ )  res += fscanf(cfgfile,"%d",&(neurons[nneu]));

	ovecsz=neurons[nlayers-1];
	//if (outsz>ovecsz)  outsz=ovecsz;

	printf("Network topology: (%d)-",ivecsz);
	for ( nlay=0; nlay<nlayers; nlay++ )
		printf("%d-",neurons[nlay]);
	printf("(%d)\n",ovecsz);

	/*... construct network topology ...*/
	create_net(BP_net);
	printf("Network created!\n");
	
	/*... read all network weights ...*/
	for(nlay=0; nlay < (BP_net->lnum); nlay++)
	{
    	for(nneu=0; nneu < (BP_net->lay[nlay].nnum); nneu++)
		{
			//res += fread(&(BP_net->lay[nlay].neu[nneu].w[0]),sizeof(TWeight),BP_net->lay[nlay].neu[nneu].wnum,cfgfile);
			for ( nwei=0; nwei<BP_net->lay[nlay].neu[nneu].wnum; nwei++ )
			//	res += fread(&(BP_net->lay[nlay].neu[nneu].w[nwei]),sizeof(TWeight),1,cfgfile);
			{ res += fscanf(cfgfile,"%f",&(BP_net->lay[nlay].neu[nneu].w[nwei]));	
			/*printf("%f   ",BP_net->lay[nlay].neu[nneu].w[nwei]);*/ }
			
			//res += fread(&(BP_net->lay[nlay].neu[nneu].h),sizeof(TWeight),1,cfgfile);
			{ res += fscanf(cfgfile,"%f",&(BP_net->lay[nlay].neu[nneu].h));
			/*printf("\n%f\n",BP_net->lay[nlay].neu[nneu].h);*/ }
		}
	}
	printf("Network weights read successfully! (items=%d)\n",res);

	free(neurons);

	return(res);
}

/*******************************************************************************************/
                    
void print_net(TNet *net)
{
    TCount nlay,  nneu, i;

    for(nlay=0; nlay < (net->lnum); nlay++)
    {
    	for(nneu=0; nneu < (net->lay[nlay].nnum); nneu++)
	{
	   for(i=0; i < (net->lay[nlay].neu[nneu].wnum); i++)
	   {
		printf("Layer #%d ,", nlay+1);
		printf(" neuron #%d ,", nneu+1);
		printf(" weight #%d = %f", i+1, net->lay[nlay].neu[nneu].w[i] );
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

 
/*******************************************************************************************/

TValue sigmoid_std(TValue val)
{
  return (TValue)(1.0 / ( 1.0 + exp(-val) )) ;
}

TValue sigmoid_tanh(TValue val)
{
  return (TValue)((exp(val)-exp(-val))/(exp(val)+exp(-val)));
}

TValue sigmoid(TValue val)
{
	switch (sigmoid_sel)
	{
		case SIGMOID_STD: /*printf("std-sigmoid ");*/ return(sigmoid_std(val)); break;
		case SIGMOID_TANH: /*printf("tanh-sigmoid ");*/ return(sigmoid_tanh(val)); break;
		default: fatal("Error: invalid sigmoid function select");
	}
}



/*******************************************************************************************/


void __inline readpat( FILE *inpfile, float *vec, int vecsize, float *dis, int dis_size)
{
    TCount i;

    for( i=0; i<vecsize; i++ )
	    fscanf(inpfile, "%f", &vec[i] );
    for( i=0; i<dis_size; i++ )
	    fscanf(inpfile, "%f", &dis[i] );

}

void __inline readinp( FILE *inpfile, float *vec, int vecsize)
{
    TCount i;

    for( i=0; i<vecsize; i++ )
	    fscanf(inpfile, "%f", &vec[i] );

}


/*******************************************************************************************/


void calc_output( TNet *bp, const float *vec)
{
    TCount 	clay, cneu, cwei;
    TValue	newval;

    //printf("Input vector:\n");
	for ( cneu=0; cneu < bp->lay[0].nnum; cneu++)	/* First layer */
	{
		//printf("(%4d):%5.3f  ",cneu+1,vec[cneu]);
	    newval = (TValue)0;
	    for( cwei=0; cwei < bp->lay[0].neu[cneu].wnum; cwei++ )
    		newval += bp->lay[0].neu[cneu].w[cwei] * vec[cwei];
	    newval += bp->lay[0].neu[cneu].h;
	    bp->lay[0].neu[cneu].eval = sigmoid( newval );
	}
	//printf("\n");

    for( clay=1; clay < bp->lnum; clay++)		/* Rest of layers */
	for(cneu=0; cneu < bp->lay[clay].nnum; cneu++)
	    {
		newval = (TValue)0;
		for( cwei=0; cwei < bp->lay[clay].neu[cneu].wnum; cwei++)
		    newval += bp->lay[clay].neu[cneu].w[cwei] * bp->lay[clay-1].neu[cwei].eval;
		newval += bp->lay[clay].neu[cneu].h;
		bp->lay[clay].neu[cneu].eval = sigmoid( newval );
	    }
}


/*******************************************************************************************/

int __inline quant( TValue fval )
{
	if (fval<=LIM0)  return(0);
	else if (fval>=LIM1)  return(1);
	else return(-1);
}


typedef struct {
	TValue   eval;
	TCount   epos;  }  Tposval;

int __cdecl compare_Tposval( const void *elem1, const void *elem2 )
{
	if ( ((Tposval*)elem1)->eval > ((Tposval*)elem2)->eval )  return(1);
	else if ( ((Tposval*)elem1)->eval < ((Tposval*)elem2)->eval )  return(-1);
	else return(0);
}

int __cdecl compare_Tposval_inv( const void *elem1, const void *elem2 )
{
	if ( ((Tposval*)elem1)->eval < ((Tposval*)elem2)->eval )  return(1);
	else if ( ((Tposval*)elem1)->eval > ((Tposval*)elem2)->eval )  return(-1);
	else return(0);
}


int output_maxpos(TNet *bp, float *dis)
{
    TCount   cneu, npos, /*mpos,*/ mval, netoutsz;//, *maxout, *disort;
	Tposval  *netout=NULL, *disout=NULL;
	int		 matched=0;
	
/*... initialize variables ...*/
	netoutsz=bp->lay[bp->lnum - 1].nnum;
	
	if ( (netout=(Tposval*)calloc(bp->lay[bp->lnum - 1].nnum,sizeof(Tposval)))==NULL )
		{ printf("Error: cannot allocate temp buffer 'netout'\n"); return(0); }

	if (dis) {
		if ( (disout=(Tposval*)calloc(bp->lay[bp->lnum - 1].nnum,sizeof(Tposval)))==NULL )
		{ printf("Error: cannot allocate temp buffer 'disout'\n"); return(0); }
	}

	//... find & sort maximum outputs ...
    for(cneu=0; cneu < netoutsz; cneu++)
	{
		netout[cneu].epos=cneu;
		netout[cneu].eval=bp->lay[bp->lnum - 1].neu[cneu].eval;

		if (dis) {
			disout[cneu].epos=cneu;
			disout[cneu].eval=dis[cneu];
		}
	}

/*
    qsort(netout,netoutsz,sizeof(Tposval),compare_Tposval_inv);
    if (dis)  qsort(disout,netoutsz,sizeof(Tposval),compare_Tposval_inv);

   //... show network & desired maximum positions ...
	if (output_sel==OUTP_MAXP) {

	maxout=(TCount*)calloc(outsz,sizeof(TCount));
	if (dis)  disort=(TCount*)calloc(outsz,sizeof(TCount));

	for ( npos=0; npos<outsz; npos++ )
	{ maxout[npos]=netout[npos].epos;
	  if (dis)  disort[npos]=disout[npos].epos; }

    for ( npos=0; npos<outsz; npos++ )
        for ( mpos=npos+1; mpos<outsz; mpos++ )
            if (maxout[mpos]>maxout[npos])
			{ mval=maxout[npos]; maxout[npos]=maxout[mpos]; maxout[mpos]=mval; }

	if (dis)  {
		for ( npos=0; npos<outsz; npos++ )
        for ( mpos=npos+1; mpos<outsz; mpos++ )
            if (disort[mpos]>disort[npos])
			{ mval=disort[npos]; disort[npos]=disort[mpos]; disort[mpos]=mval; }
	}

   //... calculate matches ...
    if (dis)  {
	mval=0;
    for ( npos=0; npos<outsz; npos++ )
        for ( mpos=0; mpos<outsz; mpos++ )
            if (maxout[mpos]==disort[npos])  mval++;
	}

	printf("%d-maxpos Network output:\n",outsz);
	for ( npos=0; npos<outsz; npos++ )
		printf("%d (%6.2f%%)  ",maxout[npos]+1,bp->lay[bp->lnum - 1].neu[maxout[npos]].eval*100.0);
	printf("\n");
	
	if (dis)  {
	printf("%d-maxpos Desired output:\n",outsz);
	for ( npos=0; npos<outsz; npos++ )
		printf("%d (%6.2f%%)  ",disort[npos]+1,dis[disort[npos]]*100.0);
	printf("\n%d-maxpos matches = %d\n\n",outsz,mval);

	mean_match=mean_match+((float)mval)/outsz;
	}
	
	
	free(maxout);  if (dis) free(disort);
	}*/
/*.........................*/
	//else /*OUTP_ABS or OUTP_QUANT*/ { 

    printf("Network output (sorted):\n");
    for ( npos=0; npos<netoutsz; npos++ )
	{
		if (output_sel==OUTP_QUANT) {
			if (quant(netout[npos].eval)>=0) printf("(%4d):%5.3f  ",netout[npos].epos+1,(float)quant(netout[npos].eval));
			else printf("(%4d): ????  ",netout[npos].epos+1);
		}
		else  printf("(%4d):%5.3f  ",netout[npos].epos+1,netout[npos].eval);
	}
    printf("\n");
	
	if (dis) {
	    printf("Desired output (sorted):\n");
		for ( npos=0; npos<netoutsz; npos++ )
		{
			if (output_sel==OUTP_QUANT) {
				if (quant(disout[npos].eval)>=0) printf("(%4d):%5.3f  ",disout[npos].epos+1,(float)quant(disout[npos].eval));
				else printf("(%4d): ????  ",disout[npos].epos+1);
			}
			else  printf("(%4d):%5.3f  ",disout[npos].epos+1,disout[npos].eval);
		}
		printf("\n");

		for ( mval=0,npos=0; npos<netoutsz; npos++ )
		{
			matched=0;
			switch (output_sel)
			{
				case OUTP_QUANT: matched=(quant(netout[npos].eval)==quant(disout[npos].eval));  break;
				case OUTP_ABS: matched=( (fabs(netout[npos].eval)-fabs(disout[npos].eval))<LIM0 );  break;
			}

			if (matched)  mval++;
		}
		printf("Matches: %d\n",mval);

		mean_match=mean_match+((float)mval)/netoutsz;  //-> sum per 'ivecs'
	}

	free(netout);
	if (dis) free(disout);
	//}

    return( mval );
}



/*******************************************************************************************/

float check_testset(TNet *bp, FILE *inpfile, int inpsize)
{
    TCount valcount;
    int vecsize, dis_size;
    float *vec=NULL, *dis=NULL;

    vecsize = /*bp->lay[0].neu[0].wnum*/ ivecsz;   					
    if (mode==MODE_EVAL)  dis_size = /*bp->lay[bp->lnum - 1].nnum*/ ovecsz;
   
    vec = (float *)calloc(vecsize, sizeof(float));
    if (mode==MODE_EVAL)  dis = (float *)calloc(dis_size, sizeof(float));
    if(!vec)  fatal("ERROR: Cannot allocate memory. Program aborted.\n");
	if (mode==MODE_EVAL)  if(!dis)  fatal("ERROR: Cannot allocate memory. Program aborted.\n");

    for(valcount = 0; valcount < inpsize; valcount++)
    {
		if (mode==MODE_EVAL) {
			printf("............ Evaluation pattern #%d of %d ............\n",valcount+1,inpsize);
			readpat(inpfile, vec, vecsize, dis, dis_size);
		}
		else {
			printf("............ Prediction pattern #%d of %d ............\n",valcount+1,inpsize);
			readinp(inpfile, vec, vecsize);
		}
    	calc_output(bp, vec);
        output_maxpos(bp, dis);
		printf("........................................................\n\n");
    }
    
	//rewind(inpfile);

    printf("Execution process completed!\n");
	if (mode==MODE_EVAL)
	{
		printf("\tMode: EVALUATION\n");
		printf("\tPatterns tested: %d , Mean Matches: %6.2f%%\n\n",inpsize,mean_match/inpsize*100.0);
	}
	else
	{
		printf("\tMode: PREDICTION\n");
		printf("\tPatterns evaluated: %d\n\n",inpsize);
	}
	printf("\tSigmoid: %s\n",((sigmoid_sel==SIGMOID_TANH)?"TanH":"Standard"));
	printf("\t Output: %s\n",((output_sel==OUTP_QUANT)?"Quantitized":"Absolute"));


    free(vec);
    if (mode==MODE_EVAL)  free(dis);
    
	return (mean_match/inpsize);  
}

/*******************************************************************************************/

void openfiles(FILE **cfgfile, FILE **inpfile )
{
   char fname[50], resp[5];
   int  cflag=0;

   
   do
   {
   	printf("Enter a valid filename for network configuration file: ");
   	scanf("%s",fname);
	//if ((*cfgfile = fopen(fname,"r+b"))==NULL)
	if ((*cfgfile = fopen(fname,"r"))==NULL)
		fatal("Error: cannot open network configuratioin file");
   }
   while(!(*cfgfile));
   
   do
   {
	   printf("Use standard or tanh sigmoid activation? (S/T): ");
	   scanf("%s",&resp);

	   if ((resp[0]=='s')||(resp[0]=='S'))  {mode=SIGMOID_STD; cflag=0;}
	   else if ((resp[0]=='t')||(resp[0]=='T'))  {mode=SIGMOID_TANH; cflag=0;}
	   else cflag=1;
   }
   while (cflag);

   do
   {
	   //printf("Use Quantitized, Absolute or Maxpos network output? (Q/A/M): ");
	   printf("Use Quantitized or Absolute network output? (Q/A): ");
	   scanf("%s",&resp);

	   if ((resp[0]=='q')||(resp[0]=='Q'))  {output_sel=OUTP_QUANT; cflag=0;}
	   else if ((resp[0]=='a')||(resp[0]=='A'))  {output_sel=OUTP_ABS; cflag=0;}
	   //else if ((resp[0]=='m')||(resp[0]=='M'))  {output_sel=OUTP_MAXP; cflag=0;}
	   else cflag=1;
   }
   while (cflag);
/*
   while ((output_sel==OUTP_MAXP)&&(outsz<=0))
   {
	   printf("Give valid output window size: ");
	   scanf("%d",&outsz);
   }
*/
   do
   {
   	printf("Enter a valid filename for input set file: ");
   	scanf("%s",fname);
	if ((*inpfile = fopen(fname,"r"))==NULL)
		fatal("Error: cannot open input file");
   }
   while(!(*inpfile));

   printf("Give number of input vectors: ");
   scanf("%d",&ivecs);

   do
   {
	   printf("Use network in Prediction or Evaluation mode? (P/E): ");
	   scanf("%s",&resp);

	   if ((resp[0]=='p')||(resp[0]=='P'))  {mode=MODE_PRED; cflag=0;}
	   else if ((resp[0]=='e')||(resp[0]=='E'))  {mode=MODE_EVAL; cflag=0;}
	   else cflag=1;
   }
   while (cflag);

}


/*******************************************************************************************/

void __inline closefiles(FILE *cfgfile, FILE *inpfile)
{
   fclose(cfgfile);
   fclose(inpfile);
}

/*******************************************************************************************/    		

#endif   /* __MLP_IMP2_H__  */
