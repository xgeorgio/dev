//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "const.h"
#include "statlib.h"
#include "mdc.h"


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


	rewind(ffrom);
    fscanf(ffrom,"%ld%d",&count,&fn);
	
	if ((count<=0)||(fn<=0))
	{
		printf("\nError: invalid patterns file header\n");
		return(0);
	}

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

//... feature-Tvalue comparison function (used by qsort()) ...
int Ptvalue_compare(const void *elem1, const void *elem2)
{
	TPtvalue		*pv1=(TPtvalue*)elem1, *pv2=(TPtvalue*)elem2;

	if (pv1->val > pv2->val)  return(-1);
	else if (pv1->val < pv2->val)  return(1);
	else  return(0);
}

//... perform T-Test in pattern features ...
long selectfeatures( double critical, int leastsel, double *tfunc, int *selection, TPtvalue *fvec, double *pattern, long patlen,long patsz )
{
	long	pcount, nrm_c=0, abn_c=0;
	int		fcount, selected=0;
	double	nrm_m=0, nrm_v=0, abn_m=0, abn_v=0;
	double	sval, tval, qval;


	*tfunc = TInv(critical,(double)patlen);

	for ( fcount=1; fcount<patsz; fcount++ )
	{
		nrm_m=0; nrm_v=0; abn_m=0; abn_v=0;
		sval=0; tval=0; qval=0;
		nrm_c=0; abn_c=0;

		for ( pcount=0; pcount<patlen; pcount++ )
		{
#ifdef	DEBUG_PRINT
			printf("pattern %ld:  class=%g\n",pcount+1,pattern[pcount*(MAX_FEATURE+1)+0]);
#endif

			if (pattern[pcount*(MAX_FEATURE+1)+0]==0.0)  { nrm_m = nrm_m + pattern[pcount*(MAX_FEATURE+1)+fcount]; nrm_c++; }
			else if (pattern[pcount*(MAX_FEATURE+1)+0]==1.0)  { abn_m = abn_m + pattern[pcount*(MAX_FEATURE+1)+fcount]; abn_c++; }
			else  continue;  //-> unknown pattern class
		}
		nrm_m = nrm_m / nrm_c;
		abn_m = abn_m / abn_c;

		for ( pcount=0; pcount<patlen; pcount++ )
		{
			if (pattern[pcount*(MAX_FEATURE+1)+0]==0.0)
				{ nrm_v = nrm_v + (pattern[pcount*(MAX_FEATURE+1)+fcount]-nrm_m)*(pattern[pcount*(MAX_FEATURE+1)+fcount]-nrm_m); }
			else if (pattern[pcount*(MAX_FEATURE+1)+0]==1.0)
				{ abn_v = abn_v + (pattern[pcount*(MAX_FEATURE+1)+fcount]-abn_m)*(pattern[pcount*(MAX_FEATURE+1)+fcount]-abn_m); }
			else  continue;  //-> unknown pattern class
		}
		nrm_v = sqrt( nrm_v/nrm_c );
		abn_v = sqrt( abn_v/abn_c );

#ifdef	DEBUG_PRINT
		{
			printf("Feature %02d:\n",fcount);
			printf("\t  Normal:  count=%ld , mean=%g , var=%g\n",nrm_c,nrm_m,nrm_v);
			printf("\tAbnormal:  count=%ld , mean=%g , var=%g\n",abn_c,abn_m,abn_v);
		}
#endif

		sval = ( nrm_v*nrm_v*(nrm_c-1) + abn_v*abn_v*(abn_c-1) ) / ( nrm_c + abn_c - 2 );
		sval = sqrt( sval );

		qval = nrm_c*abn_c/(nrm_c+abn_c);

		if ((sval==0)||(qval<0))  tval=0.0;
		else
			tval = ( fabs(nrm_m-abn_m)/sval ) * sqrt( nrm_c*abn_c/(nrm_c+abn_c) );
		
		if (tval > *tfunc)
			{ selection[fcount-1]=1;  selected++; }
		else  selection[fcount-1]=0;

#ifdef	DEBUG_PRINT
		printf("feature %-2d  ->  T-Test: tval=%6.4f (sval=%6.4f,Fdeg=%ld)\t-> %s\n",fcount,tval,sval,patlen-2,((selection[fcount-1])?"+[Selected]":" [Rejected]"));
#endif

		fvec[fcount-1].pt=fcount-1;  fvec[fcount-1].val=tval;
	}

	qsort(fvec,patsz-1,sizeof(TPtvalue),Ptvalue_compare);

	if (selected<leastsel)
	{
		for ( fcount=selected; fcount<leastsel; fcount++ )  selection[fvec[fcount].pt]=1;
		selected=leastsel;
	}

	printf("Sorted feature selection:\n");
	for ( fcount=0; fcount<patsz-1; fcount++ )
		printf("feature %02d -> tval=%6.4f\t-> %s\n",fvec[fcount].pt+1,fvec[fcount].val,((fcount<selected)?"+[Selected]":" [Rejected]"));

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
	printf("Feature combinations-2:\n");
#endif
	for ( i=0; i<max_features-1; i++ )
		for ( j=i+1; j<max_features; j++ )
		{
			FeatComb[ncombs*MAX_FEATURE+i] = 1;
			FeatComb[ncombs*MAX_FEATURE+j] = 1;
#ifdef	DEBUG_PRINT
			printf("\t%d\t%d\n",i+1,j+1);
#endif
			ncombs++;  FeatCombN[2]++;
		}

#ifdef	DEBUG_PRINT
	printf("Feature combinations-3:\n");
#endif
	for ( i=0; i<max_features-2; i++ )
		for ( j=i+1; j<max_features-1; j++ )
			for ( k=j+1; k<max_features; k++ )
			{
				FeatComb[ncombs*MAX_FEATURE+i] = 1;
				FeatComb[ncombs*MAX_FEATURE+j] = 1;
				FeatComb[ncombs*MAX_FEATURE+k] = 1;
#ifdef	DEBUG_PRINT
				printf("\t%d\t%d\t%d\n",i+1,j+1,k+1);
#endif
				ncombs++;  FeatCombN[3]++;
			}

#ifdef	DEBUG_PRINT
	printf("Feature combinations-4:\n");
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
					printf("\t%d\t%d\t%d\t%d\n",i+1,j+1,k+1,m+1);
#endif
					ncombs++;  FeatCombN[4]++;
				}

//#ifdef	DEBUG_PRINT
	printf("Total combinations:  %ld  (%ld+%ld+%ld)\n",ncombs,FeatCombN[2],FeatCombN[3],FeatCombN[4]);
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
		printf("Feature mapping:\n");
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
		printf("Saving: '%s' (#feat=%d)\n",fname,fts);
#endif

		if ((fout=fopen(fname,"w+"))==NULL)
		{ 
			printf("Error: unable to create output file ('%s')\n",fname);
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
			printf("[%d]: ",FeatConfig[cc*(MINFEATSELECT+1)+0]);
			for ( cf=0; cf<MINFEATSELECT; cf++ )  printf("%d ",FeatConfig[cc*(MINFEATSELECT+1)+cf+1]);
			printf("\n");
		}
#endif
	}

	return(cc);
}


//-------------------------------------------

int main( void )
{
	char	patfname[30], clsfname[30];
	float	ttestlim;
	double	flimit;
	FILE	*pfh, *cfh, *aux;

	int		fselect[MAX_FEATURE], SelFeatMap[MAX_FEATURE], fselcount=0, i, minsel=0;
	long	FeatCombN[MAX_FEATURE];
	int		*FeatComb;
	int		*FeatConfig;
	double	*pattern;

	int		mode=DEF_DIST, inpmode;
	long	tcombs=0, fcombs=0, wcombs=0, scount=0, sc=0, patlen=0, patsz=0;
	TPtvalue	fvec[MAX_FEATURE];
	TCPMinfo	cpm;
    
    
	printf("FCD: Feature Classifier Design, v%s (%s) - Harris Georgiou (c) 2000\n",VERSION_VER,VERSION_REL);
    printf("-------------------------------------------------------------------------\n\n");
    

//... phase 0: initialization ...

	printf("Give output patterns filename: "); scanf("%s",patfname);
	if ((pfh=fopen(patfname,"r+"))==NULL)
	{ 
		printf("Error: unable to create output file ('%s')\n",patfname);
		return(3);
	}


//... phase 1: construct optimized patterns file (selected feature set) ...
	
	printf("\nGive T-Test critical value (0.0-1.0): "); scanf("%f",&ttestlim);
	if (ttestlim<0)  ttestlim=-ttestlim;
	if (ttestlim>1)  ttestlim=1;
	
	printf("Give minimum feature selection count (0 for default): ");
	scanf("%d",&minsel);
	if (minsel<ATLEASTSELECT)  minsel=ATLEASTSELECT;

	printf("Optimizing feature selection...");
	if ((pattern=readpatterns(pfh,&patlen,&patsz))==0)  printf("\nError: unable to read patterns file ('%s')\n",patfname);
	else
	{
		printf(" done.\n");

		fselcount = selectfeatures(ttestlim,minsel,&flimit,fselect,fvec,pattern,patlen,patsz);
		printf("T-Test Results:  selected %d out of %d features (limit=%5.3f)\n",fselcount,MAX_FEATURE,flimit);
	}

	printf("Selected feature set:\n\t[%d]: ",fselcount);
	for ( i=0; i<MAX_FEATURE; i++ )  printf("%d ",fselect[i]);
	printf("\n");

  //... create DAT file (selected feature set) for ANN32 package ...
	printf("Writing AUX-1 data file ('%s')...",AUX1_FILENAME);
	if ((aux=fopen(AUX1_FILENAME,"w+"))==NULL)
		{ printf("Error: unable to create AUX-1 data file\n"); return(5);	}
	writeauxfile1(pattern,patlen,patsz,MAX_CLASS,fselect,MAX_FEATURE,aux);
	fclose(aux);
	printf(" done.\n");


//... phase-2: create feature combinations (2,3,4) ...
	tcombs=ncombs(fselcount);
	FeatComb=new int[tcombs*MAX_FEATURE];
	FeatConfig=new int[tcombs*(MINFEATSELECT+1)];
	if ((!FeatComb)||(!FeatConfig))
	{
		printf("\nError: not enough memory for feature combinations (%ld)\n",tcombs);
		if (FeatComb)  delete FeatComb;
		if (FeatConfig)  delete FeatConfig;
		fclose(pfh);
	}
	else
		printf("\nAllocated space for %ld feature combinations\n",tcombs);

	fcombs=createfeaturecombs4(fselcount,FeatComb,tcombs,FeatCombN);
	mapfeatures(fselect,SelFeatMap,MAX_FEATURE);

	printf("translating feature combinations\n");
	translatefeatcombin4(fcombs,FeatComb,SelFeatMap,MAX_FEATURE);

	printf("saving feature combinations... ");
	//wcombs=writeselcombins(PATFILE_PREFX,fcombs,patlen);
	wcombs=storeselcombins(fcombs,FeatComb,tcombs,FeatConfig);
	if (wcombs==fcombs)  printf("done (%ld configurations saved)\n",wcombs);
	else  printf("Error: (I/O) %ld of %ld configurations saved\n",wcombs,fcombs);


//... phase-3: min.dist.classifier design ...

	printf("Give distance mode (%d=Eucl,%d=Mahl): ",EUC_DIST,MAH_DIST); scanf("%d",&inpmode);
	switch(inpmode)
	{
		case EUC_DIST:
		case MAH_DIST: mode=inpmode; break;
		default:  printf("Warning: using default mode (%d)\n",mode); break;
	}
	
	printf("Give MDC output filename: "); scanf("%s",clsfname);
	if ((cfh=fopen(clsfname,"w+"))==NULL)
	{ 
		printf("Error: unable to create MDC file ('%s')\n",clsfname);
		return(5);
	}
		
	CPM_init(cpm);
	CPM_optimum(pattern,patlen,fcombs,FeatConfig,mode,cpm);

	printf("Writing optimum MDC configuration to file '%s'... ",clsfname);
	if (CPM_writetofile(cpm,cfh)==0)  printf("done.\n");
	else
	{ 
		printf("Error: unable to write to MDC file ('%s')\n",clsfname);
		return(5);
	}

	fclose(cfh);
		
  //... create DAT file (MDC-optimum feature set) for ANN32 package ...
	printf("Writing AUX-2 data file ('%s')...",AUX2_FILENAME);
	if ((aux=fopen(AUX2_FILENAME,"w+"))==NULL)
		{ printf("Error: unable to create AUX-2 data file\n"); return(5);	}
	writeauxfile2(pattern,patlen,patsz,MAX_CLASS,cpm.cls[0].fvec,cpm.cls[0].dim,aux);
	fclose(aux);
	printf(" done.\n");

//... exiting to system ...
	printf("Cleaning up... ");
	delete pattern;  delete FeatComb;  delete FeatConfig;
	printf("All done!\n\n");

    return(0);
}

