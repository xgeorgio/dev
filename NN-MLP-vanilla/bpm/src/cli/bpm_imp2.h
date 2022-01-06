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
#include "bpm_cnst.h"


//-----  General purpose utility routines  -----

//... generic (fatal) error reporting ...
void __inline fatal( const char *mes )
{
    printf("%s", mes);
    exit(1);
}


//-----  Memory buffers  -----

//... initialize buffers ...
int initialize_buffers( Tmembuf *trn_buf, Tmembuf *val_buf, Tmembuf *tst_buf )
{
	TCount   i, j;
	int		 res=0;

	
	printf("Initializing pattern set buffers...");

    //... allocate training set in/out buffers ...
	trn_buf->inpvec=(float**)calloc(trn_buf->nvec,sizeof(float*));
	if (trn_buf->inpvec)
	{
		for (i=0; i<trn_buf->nvec; i++ )
			trn_buf->inpvec[i]=(float*)calloc(trn_buf->ivecsz,sizeof(float));
	}
	if (i!=trn_buf->nvec)  fatal("Error: trn_buf(i) not allocated"); // return(-1);

	trn_buf->outvec=(float**)calloc(trn_buf->nvec,sizeof(float*));
	if (trn_buf->outvec)
	{
		for (i=0; i<trn_buf->nvec; i++ )
			trn_buf->outvec[i]=(float*)calloc(trn_buf->ovecsz,sizeof(float));
	}
	if (i!=trn_buf->nvec)  fatal("Error: trn_buf(o) not allocated"); // return(-1);

    //... allocate validation set in/out buffers ...
	val_buf->inpvec=(float**)calloc(val_buf->nvec,sizeof(float*));
	if (val_buf->inpvec)
	{
		for (i=0; i<val_buf->nvec; i++ )
			val_buf->inpvec[i]=(float*)calloc(val_buf->ivecsz,sizeof(float));
	}
	if (i!=val_buf->nvec)  fatal("Error: val_buf(i) not allocated"); // return(-1);

	val_buf->outvec=(float**)calloc(val_buf->nvec,sizeof(float*));
	if (val_buf->outvec)
	{
		for (i=0; i<val_buf->nvec; i++ )
			val_buf->outvec[i]=(float*)calloc(val_buf->ovecsz,sizeof(float));
	}
	if (i!=val_buf->nvec)  fatal("Error: val_buf(o) not allocated"); // return(-1);

    //... allocate training set in/out buffers ...
	tst_buf->inpvec=(float**)calloc(tst_buf->nvec,sizeof(float*));
	if (tst_buf->inpvec)
	{
		for (i=0; i<tst_buf->nvec; i++ )
			tst_buf->inpvec[i]=(float*)calloc(tst_buf->ivecsz,sizeof(float));
	}
	if (i!=tst_buf->nvec)  fatal("Error: tst_buf(i) not allocated"); // return(-1);

	tst_buf->outvec=(float**)calloc(tst_buf->nvec,sizeof(float*));
	if (tst_buf->outvec)
	{
		for (i=0; i<tst_buf->nvec; i++ )
			tst_buf->outvec[i]=(float*)calloc(tst_buf->ovecsz,sizeof(float));
	}
	if (i!=tst_buf->nvec)  fatal("Error: tst_buf(o) not allocated"); // return(-1);


	printf("\nReading pattern sets into buffers...");
	
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
 
    printf("\ndone!\n\n");

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
    printf("Enter training patterns file name: ");
   	scanf("%s", patfname);
    printf("Enter #training patterns: ");
    scanf("%d", npat);  

    printf("Enter validation patterns file name: ");
   	scanf("%s", valfname);
    printf("Enter #validation patterns: ");
    scanf("%d", nval);  

    printf("Enter testing patterns file name: ");
   	scanf("%s", testfname);
    printf("Enter #testing paterns: ");
    scanf("%d", ntest);  
}

//... initialize patterns files (open) ...
int initfiles( char *patfname, FILE **pat, Tmembuf *trn_buf, TCount npat,
                char *valfname, FILE **val, Tmembuf *val_buf, TCount nval,
                char *testfname, FILE **test, Tmembuf *tst_buf, TCount ntest )
{
    if ((*pat = fopen(patfname,"r"))==NULL)
    {
        printf("Error: unable to open training patterns file ('%s')\n",patfname);
        return(1);
    }

    if ((*val = fopen(valfname,"r"))==NULL)
    {
        printf("Error: unable to open validation patterns file ('%s')\n",valfname);
        return(2);
    }

    if ((*test = fopen(testfname,"r"))==NULL)
    {
        printf("Error: unable to open testing patterns file ('%s')\n",testfname);
        return(3);
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
void create_net( TNet *BP_net, TNet *aux_net, TWeight *coeff, TWeight *mcoeff,
                TCount *maxiters, TValue *goodvalptg, Tmembuf *trn_buf, Tmembuf *val_buf, Tmembuf *tst_buf )
{
    TCount      nlayers=0, nlay=0;
    TCount      nneu=0;
    TCount      nweights=0, nwei=0;
    TCount      /*neurons[MAXNEURONNUM],*/ *neurons;
    TWeight     /*weight[MAXWEIGHTNUM],*/ *weight;
    int         res=0;
                    
    printf("Give number of neuron layers: ");  scanf("%d",&nlayers);
    res = init_net( BP_net, nlayers )
		+  init_net( aux_net, nlayers );
    if (res==0)
    {
	printf("Memory for net allocated correctly!\n");
	
	printf("Give dimension of input vector: ");     scanf("%d", &nweights);

	trn_buf->ivecsz=nweights;
	val_buf->ivecsz=nweights;
	tst_buf->ivecsz=nweights;

	if ((neurons=(TCount*)calloc(nlayers,sizeof(TCount)))==NULL)
		fatal("Error: cannot allocate temp memory for neurons");

	for ( nlay=0; nlay<nlayers; nlay++ )
	{
	    printf("\tGive number of neurons in layer #%d: ",nlay+1);
	    scanf("%d",&neurons[nlay]);
	    res = init_layer( &(BP_net->lay[nlay]), neurons[nlay] ) 
	          + init_layer( &(aux_net->lay[nlay]), neurons[nlay] );
	    if (res==0)
	    {
		printf("Memory allocated for layer #%d.\n", nlay+1);

		for( nneu=0; nneu<neurons[nlay]; nneu++ )
	    	{	
			if ((weight=(TWeight*)calloc(nweights+1,sizeof(TWeight)))==NULL)
				fatal("Error: cannot allocate temp memory for weights");

		    for( nwei=0; nwei<nweights+1; nwei++ )
		 	    weight[nwei]= (TWeight)(rand() / (TWeight)RAND_MAX - 0.5);
		    res = init_neuron( &(BP_net->lay[nlay].neu[nneu]), nweights, weight, weight[nweights])
		          + init_neuron( &(aux_net->lay[nlay].neu[nneu]), nweights, weight, weight[nweights]);
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

	free(neurons);

	trn_buf->ovecsz=nweights;
	val_buf->ovecsz=nweights;
	tst_buf->ovecsz=nweights;

	printf("\nEnter the learning coefficient (0..1): ");
	scanf("%f", coeff);
    *coeff=(TValue)fabs(*coeff);

	printf("\nEnter the momentum coefficient (0..1): ");
	scanf("%f", mcoeff);
    *mcoeff=(TValue)fabs(*mcoeff);

	printf("\nEnter the maximum number of epochs (0..%d): ",MAX_ITERATIONS);
	scanf("%d", maxiters);
    *maxiters=abs(*maxiters);
    if (*maxiters>MAX_ITERATIONS)  *maxiters=MAX_ITERATIONS;

   	printf("\nEnter the validation %%success limit (%5.1f%%..%5.1f%%): ",GOOD_VAL_PTG,100.0);
	scanf("%f", goodvalptg);
    *goodvalptg=(TValue)fabs(*goodvalptg/100.0);
    if (*goodvalptg<GOOD_VAL_PTG/100.0)  *goodvalptg=(TValue)(GOOD_VAL_PTG/100.0);
    if (*goodvalptg>1.0)  *goodvalptg=(TValue)1.0;
    
    }
    else
       fatal("ERROR: Cannot allocate memory for layers\n");

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

//... save network topology & weights into file ...
long quicksave_net( TNet *opt_net, char *fname )
{
	FILE   *ax_fh;
	int    res=0;
    TCount nlay,  nneu, nwei;


	printf("Saving current network in file (%s)...\n",fname);

	ax_fh=fopen(fname,"w+");
	if (!ax_fh)
	{
		printf("Error: unable to open file '%s'... ",fname);
		return(0L);
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
void update_weights( TNet *bp, const float *vec, const float *dis, const TWeight coeff, const TWeight mcoeff )
{

    TCount	clay, cneu,	cwei, upneu;
    TWeight **error, temp;
    TValue val;

    if ((error=(TWeight**)calloc(bp->lnum,sizeof(TWeight*)))==NULL)
        fatal("Abort: weight pool allocated null.\n");

    for (clay = 0; clay < bp->lnum; clay++)
    {
	    if( (error[clay] = (TWeight*)calloc( bp->lay[clay].nnum, sizeof(TWeight)) ) == NULL )
	        fatal("ERROR: not enough memory. Program aborted.\n");
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
void print_out(TNet *bp, float *dis)
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
}
   
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
int check_validation( Tmembuf *val_buf, int valsize, TValue *meanerr, TNet *bp, TNet *aux)
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
        fatal("ERROR: Cannot allocate memory. Program aborted.\n");

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
TCount check_testset( Tmembuf *tst_buf, int testsize, TValue *meanerr, TNet *bp )
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
    if(!(vec) || !(dis) )
               fatal("ERROR: Cannot allocate memory. Program aborted.\n");

   	rewind_buffered(tst_buf);

    err = 0.0;
    for(valcount = 0; valcount < testsize; valcount++)
    {
		readpat_buffered(tst_buf, vec, vecsize, dis, dis_size);
    	calc_output(bp,vec);
    	err += calcerror(bp, dis, dis_size);
        identified += recognised(bp, dis);
    }
    
    err /= (TValue)testsize;
    //printf("\nIn val-set identified = %d mean-err = %f, ", identified, err);
    *meanerr = err;

    free(vec);
    free(dis);
    return(identified);
}

//... check test set, create truth table for 2-categories classification ...
TCount check_testset_c2( Tmembuf *tst_buf, int testsize, TValue *meanerr, TCount *result, TNet *bp )
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
               fatal("ERROR: Cannot allocate memory. Program aborted.\n");

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
void teach_net(TNet *bp, TNet *aux,
	       Tmembuf *trn_buf, TCount npat, Tmembuf *val_buf, TCount valsize, 
	       TWeight coeff, TWeight mcoeff, TCount maxiters, TValue goodvalptg )
{
    TCount vecsize, dis_size, howmany = 0, cpat=1, iterations=1;
    TValue val_percentage, meanerr;
    int learningOK = 0;
    float *vec, *dis;
    int eflag;
   

    vecsize = bp->lay[0].neu[0].wnum; 					
    dis_size = bp->lay[bp->lnum - 1].nnum;
   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis))
   	   fatal("ERROR: Cannot allocate memory. Program aborted.\n");

    //... main training loop ... 
    while (!learningOK)
    {   
        val_percentage=0;

        for (cpat=0; cpat<npat; cpat++)
        {
	        readpat_buffered(trn_buf,vec,vecsize,dis,dis_size);
            calc_output(bp,vec);
            update_weights(bp,vec,dis,coeff,mcoeff);

            howmany += recognised(bp,dis);
            val_percentage += (TValue)check_validation(val_buf,valsize,&meanerr,bp,aux);
        }	

        val_percentage = val_percentage / (TValue)(valsize*npat);

        printf("#iter=%d of %d , recogn= %d of %d, trainerr=%8.6f , validated=%6.2f%%\n", iterations, maxiters, howmany, npat, meanerr, val_percentage*100.0);
        eflag=allpaterns(trn_buf, &howmany, &cpat, npat, &iterations);
      
        if ((eflag==1)||(val_percentage>=goodvalptg)||(iterations>maxiters)) 
            learningOK = 1;

	    /*... save best network every AUTOSAVE_EVERY iterations ...*/
	    if (!(iterations % AUTOSAVE_EVERY))  quicksave_net(aux,DEFSAVNAME);
    }
    //printf("\n EFLAG: %d - HOWMANY: %d - ITERATIONS: %d\n", eflag, howmany, iterations); 

    copy_net(aux, bp);
    free(vec);
    free(dis);
}

//... network testing ...
void test_net(TNet *bp, Tmembuf *tst_buf, TCount ntest, TWeight lcoeff, TWeight mcoeff )
{

    TCount vecsize = bp->lay[0].neu[0].wnum, 
           dis_size = bp->lay[bp->lnum - 1].nnum, 
           correct = 0, result[4];
    float *vec, *dis, ptg_success, ptg_failure, meanerr;
    
    //printf("Inside test: vecsize = %d, dis_size = %d\n\n", vecsize, dis_size);   
    vec = (float *)calloc(vecsize, sizeof(float));
    dis = (float *)calloc(dis_size, sizeof(float));
    if(!(vec) || !(dis))
        fatal("ERROR: Cannot allocate memory. Program aborted.\n");
   
    correct=check_testset(tst_buf,ntest,&meanerr,bp);
    ptg_success = (float)(100.0*correct/(TValue)ntest);
    ptg_failure = (float)(100.0-ptg_success);

    printf("\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  #paterns tested        #paterns recongised         #failures            |\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  %15d        %19d         %9d            |\n", ntest, correct, ntest-correct);
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  Percentages:           %19.3f%%        %9.3f%%           |\n", ptg_success, ptg_failure); 
    printf("+--------------------------------------------------------------------------+\n");
    printf("    Learning coefficient: %5.3f     Momentum coefficient: %5.3f\n\n",lcoeff,mcoeff);

    //... restart testing, cross check results with 'hits-misses' truth table ...
    if (bp->lay[bp->lnum-1].nnum == 2)
    {
        check_testset_c2(tst_buf,ntest,&meanerr,result,bp);
        printf("\t      cN    cA  \n");
        printf("\t    +-----------+\n");
	    printf("\tpN: | %3ld | %3ld |\n",(long)result[0],(long)result[1]);
	    printf("\t    +-----------+\n");
	    printf("\tpA  | %3ld | %3ld |\n",(long)result[2],(long)result[3]);
	    printf("\t    +-----------+\n");
	    printf("\t    succ = %5.1f%%\n",((double)(result[0]+result[3]))/ntest*100);
    }

    free(vec);
    free(dis);
}

    		
#endif   /* __BPM_IMP2_H__  */
