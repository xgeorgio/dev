/*  +--------------------------------------+
    |                                      |
    |   SHUFFLE - Text file randomizer     |
    |                                      |
    |         Harris Georgiou (c) 2000     |
    |                                      |
    +--------------------------------------+
*/


//..............................................


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define     VERSION_TAG     "1.1"
#define     VERSION_REL     "R000618"

#define     MAX_LINESIZE     1000

typedef     long            TCount;



//... create random shuffle for line-blocks ...
TCount *createshuffle( TCount lines, int blocksize, TCount *shufflesize )
{
    TCount      *shuffle, lc, newpos, shflsz=lines/blocksize;

    
    shuffle=new TCount[shflsz];
    if (!shuffle)  return(NULL);

    for ( lc=0; lc<shflsz; lc++ )  shuffle[lc]=0;

    srand( (unsigned)time( NULL ) );
    for ( lc=0; lc<shflsz; lc++ )
    {
        newpos=(rand()*shflsz)/RAND_MAX;
        while (shuffle[newpos]!=0)
        {
            newpos++;
            if (newpos>=shflsz)  newpos=0;
        }
        shuffle[newpos]=lc;
    }

    (*shufflesize)=shflsz;
    printf("Shuffle order is:\n");
    for ( lc=0; lc<(*shufflesize); lc++ )  printf("%ld   ",(long)shuffle[lc]);
    printf("\n");

    return(shuffle);
}


//... count text lines, skip blank lines ...
TCount countfilelines( FILE *infile )
{
    TCount      lines=0;
    char        str[MAX_LINESIZE];

    rewind(infile);
    while(!feof(infile))
    {
        if (fgets(str,MAX_LINESIZE,infile)==NULL)  break;
        else if ((strlen(str)>0)&&(str[0]!='\n'))  lines++;
        // else  continue;
    }

    return(lines);
}


//... read text lines into buffer, skip blank lines ...
char *readfilelines( FILE *infile, TCount totallines, int blocksize, TCount *linesread )
{
    char    *buff, str[MAX_LINESIZE];
    TCount  lc=0, bc=0;
    int     error=0;

    
    buff=new char[totallines*MAX_LINESIZE];
    if (!buff)  return(NULL);

    printf("Reading file:\n");
    rewind(infile);  (*linesread)=0;
    while ((lc<totallines)&&(!error))
    {
        for ( bc=0; (bc<blocksize)&&(!error); bc++ )
        {
            if (fgets(str,MAX_LINESIZE,infile)==NULL)
                error=1;
            else if ((strlen(str)>0)&&(str[0]!='\n'))
            {
                strcpy(&(buff[lc*MAX_LINESIZE]),str);  (*linesread)++;
                lc++;  printf(".");
            }
            // else  continue;
        }
    }

    printf("\n");

    if (*linesread)  return(buff);
    else  { delete buff; return(NULL); }
}


//... write text lines to file (blank lines skipped) ...
TCount writefilelines( char *buff, TCount totallines, int blocksize, FILE *outfile )
{
    TCount  lc=0, bc=0, wlines=0;
    int     error=0;

    
    if (!buff)  return(0);

    printf("Writing file:\n");
    rewind(outfile);
    for ( lc=0; (lc<totallines)&&(!error); lc += blocksize )
        for ( bc=0; (bc<blocksize)&&(!error); bc++ )
        {
            if (fputs(&(buff[(lc+bc)*MAX_LINESIZE]),outfile)<0)
                error=1;
            else
            {
                wlines++;
                printf(".");
            }
        }

    printf("\n");
    
    return(wlines);
}


//... write shuffled text lines to file (blank lines skipped) ...
TCount writeshuffledfilelines( char *buff, TCount *shuffle, TCount shufflesize, int blocksize, FILE *outfile )
{
    TCount  lc=0, bc=0, wlines=0;
    int     error=0;

    
    if (!buff)  return(0);

    printf("Writing shuffled file:\n");
    rewind(outfile);
    for ( lc=0; (lc<shufflesize)&&(!error); lc++ )
        for ( bc=0; (bc<blocksize)&&(!error); bc++ )
        {
            //printf("writing line %ld ['%s']\n",(long)(shuffle[lc]*blocksize+bc),&(buff[(shuffle[lc]*blocksize+bc)*MAX_LINESIZE]));
            if (fputs(&(buff[(shuffle[lc]*blocksize+bc)*MAX_LINESIZE]),outfile)<0)
                error=1;
            else
            {
                wlines++;
                printf(".");
            }
        }

    printf("\n");
    
    return(wlines);
}


//.........................................

int __cdecl main( void )
{
    char        *buff;
    TCount      lines=0, rlines, wlines, *shuffle, shflsz=0;
    int         blksz=0;
    FILE        *ifh, *ofh;
    char        ifname[MAX_LINESIZE], ofname[MAX_LINESIZE];


    printf("SHUFFLE: Text file line-shuffle, v%s (%s) - Harris Georgiou (c) 2000.\n",VERSION_TAG,VERSION_REL);
    printf("---------------------------------------------------------------------------\n\n");

    printf("Give INPUT text file name: ");
    scanf("%s",ifname);
    printf("\nGive OUTPUT text file name: ");
    scanf("%s",ofname);
    printf("\nKeep 'N' lines together (block size): ");
    scanf("%d",&blksz);  blksz=abs(blksz);

    printf("Opening files:\n");
    if ((ifh=fopen(ifname,"r"))==NULL)
    {
        printf("Error: unable to open input file ('%s')\n",ifname);
        return(1);
    }
    if ((ofh=fopen(ofname,"w+"))==NULL)
    {
        printf("Error: unable to create output file ('%s')\n",ofname);
        fclose(ifh);
        return(1);
    }

    lines=countfilelines(ifh);
    if ((lines==0)||(lines%blksz))
    {
        printf("Error: unable to cluster file lines (%ld.mod.%d=%d)\n",(long)lines,blksz,lines%blksz);
        fclose(ifh); fclose(ofh);
        return(2);
    }
    printf("\tinput file: %ld lines total\n",(long)lines);

    buff=readfilelines(ifh,lines,blksz,&rlines);
    if ((buff==NULL)||(rlines!=lines))
    {
        printf("Error: unable to read from input file ('%s')\n",ifname);
        fclose(ifh); fclose(ofh); return(3);
    }

    if ((shuffle=createshuffle(lines,blksz,&shflsz))==NULL)
    {
        printf("Error: unable to create shuffle\n");
        delete buff; fclose(ifh); fclose(ofh);
        return(4);
    }

    wlines=writeshuffledfilelines(buff,shuffle,shflsz,blksz,ofh);
    if (wlines!=lines)
    {
        printf("Error: unable to write to output file ('%s')\n",ofname);
        delete buff; delete shuffle; 
        fclose(ifh); fclose(ofh);
        return(5);
    }

    printf("\nClosing files... ");
    fclose(ifh); fclose(ofh);
    delete buff;  delete shuffle;
    printf("all done!\n");

    return(0);
}
