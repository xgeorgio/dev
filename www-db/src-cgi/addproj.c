#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileman.c"

#define     MAXSTRLEN       100


void main()
{
    char    *buff, *buff2, *len1, *endptr, hexstr[3];
    int     x, y;
    long    contentlen, len;

    FILE        *fp;
    TRecord     *rec;
    char        emplcode[MAXSTRLEN], projcode[MAXSTRLEN], mweeks[MAXSTRLEN];
    char        *startptr;
    int         i, ioerr;


/*--- read data from environment ---*/
    len1=getenv("CONTENT_LENGTH");
    contentlen=strtol(len1,endptr,10);

    buff=(char*)malloc(contentlen);
    buff2=(char*)malloc(contentlen);

    fread(buff,contentlen,1,stdin);


/*--- decode HTTP format into normal text ---*/
    x=0; y=0;
    while (buff[x]!='\0')
    {
        switch(buff[x])
        {
            case '+': buff2[y]=' '; break;
            case '%': x++; strncpy(hexstr,&buff[x],2); x++;
                      if (((strcmp(hexstr,"26")==0)||(strcmp(hexstr,"3D")==0)))
                        {  buff2[y]='%'; buff2[y+1]=hexstr[0]; buff2[y+2]=hexstr[1]; y=y+2; break;  }
                      buff2[y]=(char)strtol(hexstr,NULL,16); break;
            default:  buff2[y]=buff[x]; break;
        }
        x++; y++;
    }
    buff2[y]='\0';


/*--- start HTML output ---*/
    printf("Content-type: text/html\n\n");

    printf("<HTML><HEAD><TITLE>New Employer's Working Project</TITLE></HEAD>");
    printf("<BODY BACKGROUND=\"images/WWWDB.gif\"><H2>New Employer's Working Project</H2><HR>");

    if ((fp=fopen(_DATFILE_OCCUPAT_NAME,"a+"))==NULL)
    {
        printf("<H2>ERROR: Bad data file request (file error)</H2></BODY></HTML>");
        return;
    }

    if ((rec=(TRecord*)malloc(sizeof(TRecord)))==NULL)
    {
        printf("<H2>ERROR: Server internal error (memory)</H2>");
        return;
    }

    /*... read 'emplcode' parameter ...*/
    if ((startptr=strstr(buff2,"emplcode="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        fclose(fp); return;
    }
    else
    {
        startptr=startptr+strlen("emplcode=");
        for ( i=0; (i<MAXSTRLEN-1)&&(startptr[i]!='&'); i++ )
            emplcode[i]=startptr[i];
        emplcode[i]='\0';
    }
    
    /*... read 'projcode' parameter ...*/
    if ((startptr=strstr(buff2,"projcode="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        fclose(fp); return;
    }
    else
    {
        startptr=startptr+strlen("projcode=");
        for ( i=0; (i<MAXSTRLEN-1)&&(startptr[i]!='&'); i++ )
            projcode[i]=startptr[i];
        projcode[i]='\0';
    }
    
    /*... read 'mweeks' parameter (LAST) ...*/
    if ((startptr=strstr(buff2,"mweeks="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        fclose(fp); return;
    }
    else
    {
        startptr=startptr+strlen("mweeks=");
        strncpy(mweeks,startptr,MAXSTRLEN);
    }
   
    strncpy(rec->field[0],emplcode,MAXSTRLEN);
    strncpy(rec->field[1],projcode,MAXSTRLEN);
    strncpy(rec->field[2],mweeks,MAXSTRLEN); 
    
    ioerr=appendRecord(fp,rec,_DATFILE_OCCUPAT_FIELDS);
    if (ioerr<0)
        printf("<H2>ERROR: Bad employer data request (file error)</H2>");
    else    
        printf("<H3>Employer %s project update request completed</H3>",emplcode);


    printf("</BODY></HTML>");
    
/*--- final cleanup before exit ---*/
    fclose(fp); free(rec); free(buff2);
}
    
