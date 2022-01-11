#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void main( void )
{
    char    *buff, *buff2, *endptr, hexstr[3];
    int     x, y;
    long    contentlen, len;


/*--- read data from environment ---*/
    const char *len1=getenv("CONTENT_LENGTH");
    contentlen=strtol(len1,&endptr,10);

    buff=(char*)malloc(contentlen+1);
    buff2=(char*)malloc(contentlen+1);
    
    fread(buff,contentlen,1,stdin);

/*--- decode HTTP format into normal text ---*/
    len=strlen(buff);
    for ( x=0,y=0; x<len; x++,y++ )
    {
        switch(buff[x])
        {
            case '+': buff2[x]=' '; break;
            case '%': strncpy(hexstr,&buff[x+1],2); x=x+2;
                      if (((strcmp(hexstr,"26")==0)||(strcmp(hexstr,"3D")==0)))
                        {  buff2[y]='%'; y++; strcpy(buff2,hexstr); y=y+2; break;  }
                      buff2[y]=(char)strtol(hexstr,NULL,16); beak;
            default:  buff2[y]=buff[x]; break;
        }
    }

/*--- start HTML output ---*/
    printf("Content-type: text/html\n\n");

    /*.....................*/


/*--- final cleanup before exit ---*/
    free(buff); free(buff2);
}
    