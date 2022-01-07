#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define         SCRNCOLS        64          /* "view" window width */
#define         SCRNROWS        20          /* "view window height */
#define         DATA_X          0
#define         DATA_Y          1


int main( int argc, char **argv )
{
    double      x=0, y=0;
    double      xmin, xmax, ymin, ymax, xstep, ystep;
    long        dcount, c;
    FILE        *infile;
    char        str[30], screen[25][81];      /* standard 80x25 text mode is used */
    int         pos, centerx, centery, col, lin;
    

    printf("DVIEW v2.1 - (X,Y)-series Data Viewer as text graph\n");
    if (argc<2)
    {
        printf("ERROR: You must give an input file\n");
        printf("\nUsage:   dview  <datafile.txt>\n\n");
        
        return(-1);
    }
    else if ( ( (argv[1][0]=='/')||(argv[1][0]=='-') )
            &&( (argv[1][1]=='?')||(argv[1][1]=='h')||(argv[1][1]=='H') ) )
    {
        printf("\nUsage:   dview  <datafile.txt>\n\n");
        
        return(0);
    }
    else if ((infile=fopen(argv[1],"rt"))==NULL)
    {
        printf("ERROR: Cannot open input file (%s)\n",argv[1]);
        printf("\nUsage:   dview  <datafile.txt>\n\n");
        
        return(-1);
    }
    else
    {
        printf("Reading data from input file...");

        xmax = FLT_MIN;
        xmin = FLT_MAX;
        ymax = FLT_MIN;
        ymin = FLT_MAX;
        
        dcount = 0L;  pos=DATA_X;
        while(!feof(infile))
        {
            fscanf(infile,"%s",str);
            if (feof(infile))       /* check right after data input for EOF */
                break;

            if (pos==DATA_X)
            {
                x = atof(str);
                if (x<xmin)  xmin = x;
                if (x>xmax)  xmax = x;

                pos = DATA_Y;
            }
            else    /* pos=DATA_Y */
            {
                y = atof(str);
                if (y<ymin)  ymin = y;
                if (y>ymax)  ymax = y;
                
                pos = DATA_X;
                dcount++;
            }
        }

        xstep = (xmax-xmin)/SCRNCOLS;
        ystep = (ymax-ymin)/SCRNROWS;
    }

    sprintf(&screen[0][0], "        (Y)          dview v2.1 - file: %s (%ld points)\n",argv[1],dcount);
    sprintf(&screen[1][0], "%+8.2f #---------------#---------------#---------------#---------------#     \n",ymax);
    sprintf(&screen[2][0], "         |                                                               |     \n");
    sprintf(&screen[3][0], "         |                                                               |     \n");
    sprintf(&screen[4][0], "         |                                                               |     \n");
    sprintf(&screen[5][0], "         |                                                               |     \n");
    sprintf(&screen[6][0], "%+8.2f #               .               .               .               #     \n",ymin+(SCRNROWS*ystep*3/4));
    sprintf(&screen[7][0], "         |                                                               |     \n");
    sprintf(&screen[8][0], "         |                                                               |     \n");
    sprintf(&screen[9][0], "         |                                                               |     \n");
    sprintf(&screen[10][0],"         |                                                               |     \n");
    sprintf(&screen[11][0],"%+8.2f #               .               .               .               #     \n",ymin+(SCRNROWS*ystep/2));
    sprintf(&screen[12][0],"         |                                                               |     \n");
    sprintf(&screen[13][0],"         |                                                               |     \n");
    sprintf(&screen[14][0],"         |                                                               |     \n");
    sprintf(&screen[15][0],"         |                                                               |     \n");
    sprintf(&screen[16][0],"%+8.2f #               .               .               .               #     \n",ymin+(SCRNROWS*ystep/4));
    sprintf(&screen[17][0],"         |                                                               |     \n");
    sprintf(&screen[18][0],"         |                                                               |     \n");
    sprintf(&screen[19][0],"         |                                                               |     \n");
    sprintf(&screen[20][0],"         |                                                               |     \n");
    sprintf(&screen[21][0],"%+8.2f #---------------#---------------#---------------#---------------#  (X)\n",ymin);
    sprintf(&screen[22][0],"    %+8.2f        %+8.2f        %+8.2f        %+8.2f        %+8.2f\n",xmin,xmin+(SCRNCOLS*xstep/4),xmin+(SCRNCOLS*xstep/2),xmin+(SCRNCOLS*xstep*3/4),xmax);
    sprintf(&screen[23][0],"\n");
    sprintf(&screen[24][0],"\n");

    /*fclose(infile);  infile=fopen(argv[1],"rt");*/
    rewind(infile);
    pos = DATA_X;
    while(!feof(infile))
    {
        fscanf(infile,"%s",str);
        if (feof(infile))       /* check right after data input for EOF */
            break;

        if (pos==DATA_X)
        {
            x = atof(str);
            pos = DATA_Y;
        }
        else    /* pos=DATA_Y */
        {
            y = atof(str);
            pos = DATA_X;
        
            col = (int)((x - xmin)/xstep)+9;
            lin = SCRNROWS+1-(int)((y - ymin)/ystep);
            screen[lin][col] = '*';
        }
    }

    printf("done!\n\n");
    for ( col=0; col<24; col++ )
        printf("%s",screen[col]);

    fclose(infile);
    return(0);
}

