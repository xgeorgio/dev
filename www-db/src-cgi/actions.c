#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileman.c"

#define     MAXSTRLEN       1000


void main()
{
    char    *buff, *buff2, hexstr[3];
    int     x, y;

    char        emplcode[MAXSTRLEN], emplname[MAXSTRLEN], emplbio[MAXSTRLEN];
    char        *startptr;
    int         i;


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

    printf("<HTML><HEAD><TITLE>Actions List</TITLE></HEAD>");
    printf("<BODY BACKGROUND=\"images/WWWDB.gif\"><H2>Actions List</H2><HR>");

    /*... read 'emplcode' parameter ...*/
    if ((startptr=strstr(buff2,"emplcode="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        return;
    }
    else
    {
        startptr=startptr+strlen("emplcode=");
        for ( i=0; (i<MAXSTRLEN-1)&&(startptr[i]!='&'); i++ )
            emplcode[i]=startptr[i];
        emplcode[i]='\0';
    }
    
    /*... read 'emplname' parameter ...*/
    if ((startptr=strstr(buff2,"emplname="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        return;
    }
    else
    {
        startptr=startptr+strlen("emplname=");
        for ( i=0; (i<MAXSTRLEN-1)&&(startptr[i]!='&'); i++ )
            emplname[i]=startptr[i];
        emplname[i]='\0';
    }
    
    /*... read 'emplbio' parameter (LAST) ...*/
    if ((startptr=strstr(buff2,"emplbio="))==NULL)
    {
        printf("<H2>ERROR: Bad employer data request (invalid parameter)</H2></BODY></HTML>");
        return;
    }
    else
    {
        startptr=startptr+strlen("emplbio=");
        strncpy(emplbio,startptr,MAXSTRLEN);
    }
    
    
    printf("<H2>%s : %s</H2><HR>",emplcode,emplname);
    printf("Actions:<P><UL><LI><IMG SRC=\"images/URLLINK.gif\"><A HREF=\"%s\" TARGET=\"FR_ACTIONS\">View biographic information</A></LI>",emplbio);
    printf("<LI><IMG SRC=\"images/URLLINK.gif\"><A HREF=\"showproj.cgi?emplcode=%s&emplname=%s\" TARGET=\"FR_ACTIONS\">Show working projects</A></LI>",emplcode,emplname);
    printf("<LI><IMG SRC=\"images/URLLINK.gif\"><A HREF=\"addpform.cgi?emplcode=%s&emplname=%s\" TARGET=\"FR_ACTIONS\">Add new working projects</A></LI></UL>",emplcode,emplname);

    printf("</BODY></HTML>");
    
}
    
