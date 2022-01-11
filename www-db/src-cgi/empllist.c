#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileman.c"


void main()
{
    FILE        *fp;
    TRecord     *rec;
    int         ioerr, c;

/*--- start HTML output ---*/
    printf("Content-type: text/html\n\n");

    printf("<HTML><HEAD><TITLE>Employers List</TITLE></HEAD>");
    printf("<BODY BACKGROUND=\"images/WWWDB.gif\"><H2>Employers List</H2><HR>");
    
    if ((fp=fopen(_DATFILE_EMPLOYER_NAME,"r"))==NULL)
        printf("<H2>ERROR: Bad data file request (file error)</H2>");
    else if ((rec=(TRecord*)malloc(sizeof(TRecord)))==NULL)
        printf("<H2>ERROR: Server internal error (memory)</H2>");
    else
    {
        printf("<TABLE BORDER><TR><TH>Code</TH><TH>Name</TH></TR>\n");
        ioerr=0;
        while(!ioerr)
        {
            ioerr=readLine(fp,rec,_DATFILE_EMPLOYER_FIELDS);

            if (ioerr) break;
            printf("<TR><TD>%s</TD><TD>\n",rec->field[0]);
            printf("<A HREF=\"actions.cgi?emplcode=%s&emplname=%s&emplbio=%s\"\n",rec->field[0],rec->field[1],rec->field[2]);
            printf(" TARGET=\"FR_CHOICES\">%s</A></TD></TR>\n",rec->field[1]);
        }
        printf("</TABLE>");
    }

    printf("</BODY></HTML>");

/*--- final cleanup before exit ---*/
    /*fclose(fp);*/  if (rec) free(rec);
}
    
