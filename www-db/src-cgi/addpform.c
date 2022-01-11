#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileman.c"

#define     MAXSTRLEN       100


void main()
{
    char    *buff, *buff2, hexstr[3];
    int     x, y;

    FILE        *fp;
    TRecord     *rec;
    char        emplcode[MAXSTRLEN], emplname[MAXSTRLEN];
    char        *startptr;
    int         i, ioerr;


/*--- read data from environment ---*/
    buff=getenv("QUERY_STRING");
    buff2=(char*)malloc(MAXSTRLEN*sizeof(char));

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

    if ((fp=fopen(_DATFILE_PROJECT_NAME,"r"))==NULL)
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
    
    /*... read 'emplname' parameter (LAST) ...*/
    if ((startptr=strstr(buff2,"emplname="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        fclose(fp); return;
    }
    else
    {
        startptr=startptr+strlen("emplname=");
        strncpy(emplname,startptr,MAXSTRLEN);
    }
    
    
    printf("<FORM METHOD=POST ACTION=\"addproj.cgi\"><TABLE>");
    printf("<TR><TH>Employer code:</TH><TD><INPUT TYPE=\"TEXT\" NAME=\"emplcode\" VALUE=%s HIDDEN></TD>",emplcode);
    printf("<TR><TH>Project:</TH><TD><SELECT NAME=\"projcode\" SIZE=1>");
    ioerr=0;
    while(!ioerr)
    {
        ioerr=readLine(fp,rec,_DATFILE_PROJECT_FIELDS);
        if (ioerr)  break;
        printf("<OPTION>%s",rec->field[0]);
    }
    printf("</SELECT></TD></TR>");
    printf("<TR><TH>Time spent (weeks):</TH><TD><INPUT TYPE=\"TEXT\" NAME=\"mweeks\"></TD>");
    printf("</TABLE><HR>");
    printf("<INPUT TYPE=\"SUBMIT\" VALUE=\"Submit\">");
    printf("<INPUT TYPE=\"RESET\" VALUE=\"Clear\">");
    printf("</FORM>");


    printf("</BODY></HTML>");
    
/*--- final cleanup before exit ---*/
    fclose(fp); free(buff); free(buff2); free(rec);
}
    
