//-------------------------------------------------
//  
//   File: Sampled Image Feature Extractor, v2.0
//
//-------------------------------------------------
//                      Harris Georgiou, 2000.
//-------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "const.h"
#include "features.h"
#include <conio.h>


typedef struct tagPoint {
	int		x, y;		} TPoint;

typedef struct tagPtvalue {
	int		pt;
	double	val;		} TPtvalue;


//.....................................

int *readimage( char *filename )
{
    int		header[BMP_header];
	FILE	*fp;
    int		i, j, *im;


    im=new int[xx*yy];
	if (!im)  return(NULL);

    fp=fopen(filename,"rb");
    if (fp==NULL)
    {  printf("Error: Unable to open file ('%s')\n",filename); return(NULL);  }

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
		printf("Error: sample box out of range\n");
#endif
		return(1);
	}

    for ( i=0; i<sx; i++ )
        for ( j=0; j<sy; j++ )
            *(box+i+j*sx) = *(img+(cx+i)+(cy+j)*xx);

	return(0);
}

//... Calculate Spatial Dependence Matrix (SDM) for image region ...
int *calculate_sdm( int *box_dat, int sx, int sy, int gsc )
{
	int		i, j, sdm_dirs=0, pgv, cgv;
	int		*box_sdm=new int[gsc*gsc];


	if (!box_sdm)
	{
		printf("Error: unable to allocate SDM buffer\n");
		return(NULL);
	}

	//... initialize SDM matrices ...
#ifdef	DEBUG_PRINT
	printf("calc.SDM...");
#endif

	for ( i=0; i<gsc; i++ )
		for ( j=0; j<gsc; j++ )
			*(box_sdm+i+j*gsc)=0;

	//... SDM calculations for 'N->S' direction ...
#ifdef	DEBUG_PRINT
	printf("[N:S");
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
	printf(",W:E]...");
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
				printf("Error: SDM index out of range\n");
				delete box_sdm;  return(NULL);
			}
			(*(box_sdm + pgv + cgv*gsc))++;
		}
	}

	//... Averaging SDM calculations for all directions ...
#ifdef	DEBUG_PRINT
	printf("[avg]...");
#endif

	for ( i=0; i<gsc; i++ )
		for ( j=0; j<gsc; j++ )
			*(box_sdm+i+j*gsc)  = (int)(*(box_sdm+i+j*gsc)/(double)sdm_dirs);

#ifdef	DEBUG_PRINT
	printf("done.\n");
#endif

	return(box_sdm);
}


//... Calculate Run-Length Matrix (RLM) for image region ...
int *calculate_rlm( int *box_dat, int sx, int sy, int gsc )
{
	int		i, j, rlm_dirs=0, pgv, cgv, crl, boxsz=max(sx,sy);
	int		*box_rlm=new int[gsc*boxsz];


	if (!box_rlm)
	{
		printf("Error: unable to allocate RLM buffer\n");
		return(NULL);
	}

	//... initialize RLM matrices ...
#ifdef	DEBUG_PRINT
	printf("calc.RLM...");
#endif

	for ( j=0; j<gsc; j++ )
		for ( i=0; i<boxsz; i++ )
			*(box_rlm+i+j*boxsz)=0;

	//... RLM calculations for 'N->S' direction ...
#ifdef	DEBUG_PRINT
	printf("[N:S");
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
					printf("Error: RLM index out of range\n");
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
	printf(",W:E]...");
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
					printf("Error: RLM index out of range\n");
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
	printf("[avg]...");
#endif

	for ( j=0; j<gsc; j++ )
		for ( i=0; i<boxsz; i++ )
			*(box_rlm+i+j*boxsz)=(int)(*(box_rlm+i+j*boxsz)/(double)rlm_dirs);
	
#ifdef	DEBUG_PRINT
	printf("done.\n");
#endif
	
	return(box_rlm);
}
	
	
//... create normal & abnormal samples, calculate features ...
int createpatterns(FILE *fout, int *ns_dat, int sx, int sy, int gscale, double noiselev, double bias, int pat_types )
{
	int		i, j, boxsz=max(sx,sy);
    int     asp_dat[MAX_BOXSIZE*MAX_BOXSIZE], asm_dat[MAX_BOXSIZE*MAX_BOXSIZE];
    int     *ns_sdm, *asp_sdm, *asm_sdm, sdm_dirs=0;
	int		*ns_rlm, *asp_rlm, *asm_rlm, rlm_dirs=0, crl=0;
	double	nsval, bsval, FeatValue[MAX_FEATURE];


	//... Noisy sampler: create normal & abnormal (+/-) boxes ...
#ifdef	DEBUG_PRINT
	printf("\tNew pattern(s): sampling...");
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
			printf("Value = %d",*(ns_dat+i+j*sx));
			if (pat_types & PAT_ASP) printf(" , Noisy(+) = %d (%+4.2f%%)",*(asp_dat+i+j*sx),nsval*100);
			if (pat_types & PAT_ASM) printf(" , Noisy(-) = %d (%+4.2f%%)\n",*(asm_dat+i+j*sx),-nsval*100);
            printf("\n");
#endif
		}

	//***** calculate spatial dependence matrices (SDM) for samples *****
	if ((ns_sdm=calculate_sdm(ns_dat,sx,sy,gscale))==NULL)
	{
		printf("\nError: unable to calculate SDM for 'ns' sample.\n");
		return(1);
	}
	
	if (pat_types & PAT_ASP)
        if ((asp_sdm=calculate_sdm(asp_dat,sx,sy,gscale))==NULL)
	    {
		    printf("\nError: unable to calculate SDM for 'asp' sample.\n");
		    return(1);
	    }

	if (pat_types & PAT_ASM)
        if ((asm_sdm=calculate_sdm(asm_dat,sx,sy,gscale))==NULL)
	    {
		    printf("\nError: unable to calculate SDM for 'asm' sample.\n");
		    return(1);
	    }

	//***** calculate run-length matrices (RLM) for samples *****
	if ((ns_rlm=calculate_rlm(ns_dat,sx,sy,gscale))==NULL)
	{
		printf("\nError: unable to calculate RLM for 'ns' sample.\n");
		return(1);
	}
	
	if (pat_types & PAT_ASP)
	    if ((asp_rlm=calculate_rlm(asp_dat,sx,sy,gscale))==NULL)
	    {
		    printf("\nError: unable to calculate RLM for 'asp' sample.\n");
		    return(1);
	    }

	if (pat_types & PAT_ASM)
	    if ((asm_rlm=calculate_rlm(asm_dat,sx,sy,gscale))==NULL)
	    {
		    printf("\nError: unable to calculate RLM for 'asm' sample.\n");
		    return(1);
	    }

	//***** Calculate features for all samples *****
#ifdef	DEBUG_PRINT
	printf("done\nCalculating features...[ns]...");
#endif
	// calculate features for normal (ns) box
	calcfeatures(ns_dat,sx,sy,ns_sdm,gscale,gscale,ns_rlm,gscale,boxsz,FeatValue);
	fprintf(fout,"%3.1f\t",0.0);
	for ( i=0; i<MAX_FEATURE; i++ )  fprintf(fout,"%8.3f\t",FeatValue[i]);
	fprintf(fout,"\n");

	if (pat_types & PAT_ASP)
    {
#ifdef	DEBUG_PRINT
	    printf("[asp]...");
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
    	printf("[asm]...");
#endif
	    // calculate features for noisy (asp) box
	    calcfeatures(asm_dat,sx,sy,asm_sdm,gscale,gscale,asm_rlm,gscale,boxsz,FeatValue);
	    fprintf(fout,"%3.1f\t",1.0);
	    for ( i=0; i<MAX_FEATURE; i++ )  fprintf(fout,"%8.3f\t",FeatValue[i]);
	    fprintf(fout,"\n");
    }

#ifdef	DEBUG_PRINT
	printf("done\n");
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
double * readpatterns( FILE *ffrom, long *patlen, long *patsize )
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
		printf("\nError: unable to allocate patterns buffer\n");
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

	printf("\npatterns read:  count=%ld , features=%d\n",pc,t-1);

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
		else
			printf("\nWarning: unable to normalize feature %ld\n",fc);
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
		else
			printf("\nWarning: unable to normalize feature %ld\n",fc);
	}

	delete minval;  delete maxval;
	return(fc);
}


//-------------------------------------------

int main( void )
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

	long	tcombs=0, fcombs=0, wcombs=0, scount=0, sc=0, patlen=0, patsz=0;
    
    
	printf("SIFX: Sample Image Feature Extractor, v%s (%s) - Harris Georgiou (c) 2000\n",VERSION_VER,VERSION_REL);
    printf("-------------------------------------------------------------------------------\n\n");
    

//... phase 0: initialization ...

	//... initialize image buffers ...
	box=new int[MAX_BOXSIZE*MAX_BOXSIZE];
	if (!box)
	{
		printf("Error: unable to allocate image buffers.\n");
		if (box)  delete box;
		return(1);
	}

	//... read bitmap and create normalized greyscaled image ...
	printf("Give input (image) filename: "); scanf("%s",bmpfname);
    if ((imdata=readimage(bmpfname))==NULL)  return(1);
    printf("Image read OK\n");
    
	printf("Give image scale range (2-%d): ",GSCALE_VALUES); scanf("%d",&gscale);
	if (gscale<2)  { gscale=2; printf("Warning: image range scale changed to %d\n",gscale); }
	else if (gscale>GSCALE_VALUES)  { gscale=GSCALE_VALUES; printf("Warning: image range scale changed to %d\n",gscale); }
    if ((img=gsn_image(imdata,gscale))==NULL)  return(1);
    printf("Image normalized OK\n");

	//... read sample points and box size ...
	printf("Give samples pt file: "); scanf("%s",samfname);
	if ((sfh=fopen(samfname,"r"))==NULL)
	{ 
		printf("Error: unable to open samples file ('%s')\n",samfname);
		return(2);
	}
		
	//... open & prepare sample pt data file ...
	fscanf(sfh,"%ld%d",&scount,&bsz.x);
	if ((bsz.x<0)||(bsz.x>MAX_BOXSIZE))
	{
		bsz.x=MAX_BOXSIZE;
		printf("Warning: sample image window changed to %dx%d pts\n",bsz.x,bsz.x);
	}
	bsz.y=bsz.x;
	printf("Creating: %ldx[%dx%d] image samples\n",scount,bsz.x,bsz.y);

	//... create main patterns file ...
	printf("Give output patterns filename: "); scanf("%s",patfname);
	if ((pfh=fopen(patfname,"w+"))==NULL)
	{ 
		printf("Error: unable to create output file ('%s')\n",patfname);
		return(3);
	}

	//... create temporaty (work) patterns file ...
	if ((ftmp=fopen(TMPFILENAME,"w+"))==NULL)
	{ 
		printf("Error: unable to create work file\n");
		return(4);
	}

	//... get noise parameters for simulated data ...
	printf("Give noise bias (0.0 ... +1.0): "); scanf("%f",&bias);
	//printf("bias=%g\n",bias);
	//if (bias<0)  bias=-bias;
	
	printf("Give noise level (0.0 ... +1.0): "); scanf("%f",&noisev);
	//printf("noisev=%g\n",noisev);
	//if (noisev<0)  noisev=-noisev;
	
    printf("Enter type(s) of simulated data to create:\n");
    printf("\t(%d=NONE, %d=ASP, %d=ASM, %d=BOTH):  ",PAT_NONE,PAT_ASP,PAT_ASM,(PAT_ASP|PAT_ASM));
    scanf("%d",&pat_types);
    switch(pat_types)
    {
        case PAT_NONE:
        case PAT_ASP:
        case PAT_ASM:
        case (PAT_ASP|PAT_ASM):  break;
        default: printf("Warning: type changed to default (%d)\n",PAT_ASP);
                 pat_types=PAT_ASP; break;
    }

	
//... phase 1: read image samples & calculate features ...

	//... create normal & simulated data, store into work file ...
	printf("\nCreating training patterns:\n");
	while ((sc<scount)&&(!feof(sfh)))
	{
		fscanf(sfh,"%d%d",&(cnt.x),&(cnt.y));

		if ((cnt.x<0)||(cnt.y<0))  continue;

		copysample(img,cnt.x,cnt.y,bsz.x,bsz.y,box);
		createpatterns(ftmp,box,bsz.x,bsz.y,gscale,noisev,bias,pat_types);

		printf(".");  sc++;
	}
    printf("\n");


//... phase 2: construct patterns file (full feature set) ...
	
	//... convert work file into main patterns file ...
	printf("Constructing patterns file...");
	if (convertpatternfile(ftmp,pfh)==0)  printf("\nError: unable to write to output file ('%s')\n",patfname);
	else  printf(" done.\n");

	fclose(ftmp);  remove(TMPFILENAME);

	//... read & normalize main pattern file ...
	printf("Reading initial patterns file...");
	if ((pattern=readpatterns(pfh,&patlen,&patsz))==0)  printf("\nError: unable to read patterns file ('%s')\n",patfname);
	else
	{
		printf(" done.\n");
		printf("Writing normalized patterns file...");
		if (normpatterns(pattern,patlen,patsz)==0)
			printf("\nWarning: unable to normalize feature vectors.\n");
		else
			printf(" done.\n");
	}
	
	fclose(pfh);
	
	//... write final (normalized) patterns file ...
	if ((ftmp=fopen(TMPFILENAME,"w+"))==NULL)
	{ 
		printf("Error: unable to create work file\n");
		return(4);
	}

	printf("Constructing normalized patterns file...");
	if (writepatternfile(pattern,patlen,patsz,ftmp)==0)
	{
		printf("\nError: unable to write to output file ('%s')\n",patfname);
		fclose(ftmp);
	}
	else
	{
		fclose(ftmp);
		if (remove(patfname))
			printf("\nError: unable to delete output file ('%s')\n",patfname);
		else
		{
			if (rename(TMPFILENAME,patfname))
				printf("\nError: unable to rename output file ('%s')\n",TMPFILENAME);
			else
				printf(" done.\n");
		}
	}

	//... create DAT file (full feature set) for ANN32 package ...
	printf("Writing AUX-0 data file ('%s')...",AUX0_FILENAME);
	if ((aux=fopen(AUX0_FILENAME,"w+"))==NULL)
		{ printf("Error: unable to create AUX-0 data file\n"); return(5);	}
	writeauxfile0(pattern,patlen,patsz,MAX_CLASS,aux);
	fclose(aux);
	printf(" done.\n");


//... exiting to system ...
	
	//... close files & free memory buffers ...
	printf("Cleaning up... ");
	delete pattern;
	delete imdata;  delete img;  delete box;
	fclose(pfh);
	printf("All done!\n\n");

    return(0);
}

