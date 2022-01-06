//-------------------------------------------------
//  
//   File: Feature Classifier Design, v1.0
//
//-------------------------------------------------
//               Harris Georgiou, 2000.
//-------------------------------------------------


#ifndef		_MDC_H
#define		_MDC_H

#include <math.h>
#include <float.h>
#include <memory.h>
#include "const.h"

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


double CPM_optimum( double *pattern, long pcount, /*int **select,*/ long slen, int *FeatConfig, int mode, TCPMinfo &cpm )
{
	long		csel, bestsel=0, pc;
	int			fsel[MINFEATSELECT], flen, corrcls, selcls, c, fc;
	TCPMinfo	cpmtmp, cpmbest, cpm1;
	double		succ, bestsucc=0, fvec[MINFEATSELECT];


	printf("\nOPTIMIZING MDC CONFIGURATION:\n");
	CPM_init(cpmtmp); CPM_init(cpmbest);

	for ( csel=0; csel<slen; csel++ )
	{
		for ( fc=0; fc<MINFEATSELECT; fc++ )
			fsel[fc]=FeatConfig[csel*(MINFEATSELECT+1)+fc+1];
		flen=FeatConfig[csel*(MINFEATSELECT+1)+0];

		succ=CPM_create(pattern,pcount,fsel,flen,mode,cpmtmp);
		if (succ>bestsucc)
		{
			bestsucc=succ;  bestsel=csel;
			CPM_copy(cpmtmp,cpmbest);
			printf("CHANGED (config=%ld): Correct = %7.3f%%\n",csel+1,bestsucc*100);
			CPM_printalltostream(cpmbest,stdout);
		}
	}

	printf("\nBEST CONFIGURATION:\n");
	CPM_printalltostream(cpmbest,stdout);

	//... create (full) optimum classifier ...
	printf("\nOPTIMUM FEATURE SELECTION:  { ");
	for ( fc=0; fc<FeatConfig[bestsel*(MINFEATSELECT+1)+0]; fc++ )
	{
		fsel[fc]=FeatConfig[bestsel*(MINFEATSELECT+1)+fc+1];
		printf("%d ",fsel[fc]);	
	}
	printf("}\n");
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

	printf("\nOPTIMUM MDC:\n");
	CPM_printalltostream(cpm1,stdout);

	CPM_copy(cpm1,cpm);
	return(succ);
}



#endif




