#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define     _ERROR_NONE         0
#define     _ERROR_IO           -1
#define     _ERROR_PARAM        -2
#define     _ERROR_MEM          -3
#define     _ERROR_NOTFOUND     -10
#define     _ERROR_DUPLICAT     -11

#define     MAXFIELD        3
#define     MAXFIELDLEN     100

#define     _NULLFIELD      "---EMPTY---"
#define     _DATFILE_EMPLOYER_NAME     "employer.dat"
#define     _DATFILE_OCCUPAT_NAME      "occupat.dat"
#define     _DATFILE_PROJECT_NAME      "project.dat"

#define     _DATFILE_EMPLOYER_FIELDS   3
#define     _DATFILE_OCCUPAT_FIELDS    3
#define     _DATFILE_PROJECT_FIELDS    1


struct record_t {
    char    field[MAXFIELD][MAXFIELDLEN];  };

typedef struct record_t     TRecord;

/*****   raw read (line)   *****/
int readLine(fp,rec,fields)
FILE *fp;
TRecord *rec;
int fields;
{
    int     status=0;
    
    if ((fp==NULL)||(rec==NULL)||(fields<=0)||(fields>MAXFIELD))
        return(_ERROR_PARAM);
    
    clearerr(fp);
    switch (fields)
    {
        case 1: status=status+fscanf(fp,"%s",rec->field[0]); rec->field[1][0]=NULL; rec->field[2][0]=NULL; rec->field[3][0]=NULL; break;
        case 2: status=status+fscanf(fp,"%s%s",rec->field[0],rec->field[1]); rec->field[2][0]=NULL; rec->field[3][0]=NULL; break;
        case 3: status=status+fscanf(fp,"%s%s%s",rec->field[0],rec->field[1],rec->field[2]); rec->field[3][0]=NULL; break;
        case 4: status=status+fscanf(fp,"%s%s%s%s",rec->field[0],rec->field[1],rec->field[2],rec->field[3]); break;
        default: return(_ERROR_PARAM);
    }
    if (status!=fields)  return(_ERROR_IO);

    return(_ERROR_NONE);
}


/*****   raw write (line)   *****/
int writeLine(fp,rec,fields)
FILE *fp;
TRecord *rec;
int fields;
{
    int     status=0, ok=0;
    
    if ((fp==NULL)||(rec==NULL)||(fields<=0)||(fields>MAXFIELD))
        return(_ERROR_PARAM);
    
    clearerr(fp);
    switch (fields)
    {
        case 1: /*printf("* ");*/ status=fprintf(fp,"%s\n",rec->field[0]); ok=(status>=0); break;
        case 2: /*printf("** ");*/ status=fprintf(fp,"%s\t%s\n",rec->field[0],rec->field[1]); ok=(status>=0); break;
        case 3: /*printf("*** ");*/ status=fprintf(fp,"%s\t%s\t%s\n",rec->field[0],rec->field[1],rec->field[2]); ok=(status>=0); break;
        case 4: /*printf("**** ");*/ status=fprintf(fp,"%s\t%s\t%s\t%s\n",rec->field[0],rec->field[1],rec->field[2],rec->field[3]); ok=(status>=0); break;
        default: return(_ERROR_PARAM);
    }
    if (!ok)  return(_ERROR_IO);

    /*printf("ok\n");*/
    return(_ERROR_NONE);
}


/*****   find record (field match) and return all fields   *****/
int findRecord(fp,key,keyfield,rec,fields )
FILE *fp;
char *key;
int keyfield;
TRecord *rec;
int fields;
{
    int     ioerr=0, found=0, recnum=0;

    
    if ((fp==NULL)||(rec==NULL)||(key==NULL)
        ||(fields<=0)||(fields>MAXFIELD)||(keyfield<=0)||(keyfield>fields))
        return(_ERROR_PARAM);
    
    rewind(fp);
    while ((!ioerr)&&(!found))
    {
        ioerr=readLine(fp,rec,fields);
        if (strcmp(rec->field[keyfield-1],key)==0)  found=1;
        recnum++;
    }

    if (ioerr)  return(ioerr);
    if (!found) return(_ERROR_NOTFOUND);

    return(recnum);
}


/*****   append new record at the end of the file   *****/
int appendRecord(fp,rec,fields)
FILE *fp;
TRecord *rec;
int fields;
{
    int     ioerr=0, c=0;
    TRecord *trec;


    if ((fp==NULL)||(rec==NULL)||(fields<=0)||(fields>MAXFIELD))
        return(_ERROR_PARAM);

    if ((trec=(TRecord*)malloc(sizeof(TRecord)))==NULL)  return(_ERROR_MEM);

    rewind(fp);
    while ((!ioerr)&&(!feof(fp)))
    {
        ioerr=readLine(fp,trec,fields);
        if (ioerr)  break;
        c++;
    }
    free(trec);

    /*printf("*** appending new slot ***\n");*/
    ioerr=writeLine(fp,rec,fields);
    if (ioerr)  return(ioerr);
    c++;

    return(c);
}


/**********************************************

void main()
{
    FILE    *fp;
    int     ioerr=0, c;
    TRecord *rec;
    

    if ((fp=fopen(_DATFILE_EMPLOYER_NAME,"r+"))==NULL)
    {
        printf("ERROR: cannot open file (%s)\n",_DATFILE_EMPLOYER_NAME);
        return;
    }

    if ((rec=(TRecord*)malloc(sizeof(TRecord)))==NULL)
    {
        printf("ERROR: cannot allocate buffer\n");
        return;
    }

    printf("File: %s\n---------------------------\n",_DATFILE_EMPLOYER_NAME);
    while (!ioerr)
    {
        ioerr=readLine(fp,rec,_DATFILE_EMPLOYER_FIELDS);
        if (!ioerr)
        for ( c=0; c<_DATFILE_EMPLOYER_FIELDS; c++ )
            printf("%s\t",rec->field[c]);
        printf("\n");
    }


    printf("Find employee: 'Ioannou'\n");
    if (findRecord(fp,"Ioannou",2,rec,_DATFILE_EMPLOYER_FIELDS)>=0)
    {
        for ( c=0; c<_DATFILE_EMPLOYER_FIELDS; c++ )
            printf("%s\t",rec->field[c]);
        printf("\n");
    }
    else
        printf("not found (?)\n");

    free(rec);
}
**********/
