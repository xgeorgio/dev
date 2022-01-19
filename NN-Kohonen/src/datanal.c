/*    ******************************************************************
      ***                                                            ***
      ***         Artificial Neural Networks - Kohonen model         ***
      ***         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~         ***
      ***                                                            ***
      ***                                                            ***
      ******************************************************************



        DESCRIPTION:
                        
            This file contains all source code needed to implement some
          statistical functions. They are used to analyze the random data
          set for input for the ANN.


------------------------------------------------------------------------------
*/

#include <math.h>
#include <stdlib.h>


void randomize_array( double *arr, int num, double maxval )
{
    int         i;

    /* randomize(); */
    for ( i=0; i<num; i++ )
        arr[i] = maxval*((double)rand()/RAND_MAX);
}



/*void randomize_array2D( double **arr, int num1, int num2, double maxval )
{
    int         i1, i2;

    randomize();
    for ( i1=0; i1<num1; i1++ )
        for ( i2=0; i2<num2; i2++ )
            arr[i1][i2] = maxval*((double)rand()/RAND_MAX);
}*/


double stat_avg( double *data, int num )
{
    double      sum;
    int         i;

    for ( i=0,sum=0.0; i<num; i++ )
        sum = sum + data[i];

    return( sum/num );
}


double stat_devia( double *data, int num )
{
    double      sum, avg;
    int         i;

    avg = stat_avg( data, num );
    for ( i=0,sum=0.0; i<num; i++ )
        sum = sum + (data[i]-avg)*(data[i]-avg);

    return( sum/num );
}


double stat_stdev( double *data, int num )
{
    double      res;

    res = sqrt( stat_devia(data,num) );
    return( res );
}


/*void stat_avg2D( double *datax, double *datay, int num, double *avg )
{
    double      sum[2];
    int         i;

    sum[0] = 0.0;
    sum[1] = 0.0;
    for ( i=0; i<num; i++ )
    {
        sum[0] = sum[0] + datax[i];
        sum[1] = sum[1] + datay[i];
    }

    avg[0] = sum[0]/num;
    avg[1] = sum[1]/num;
}*/

/*void stat_devia2D( double *datax, double *datay, int num, double *dev )
{
    double      sum[2], avg[2];
    int         i;

    stat_avg2D( datax, datay, num, avg );
    sum[0] = 0.0;
    sum[1] = 0.0;
    for ( i=0; i<num; i++ )
    {
        sum[0] = sum[0] + (datax[i]-avg[0])*(datax[i]-avg[0]);
        sum[1] = sum[1] + (datay[i]-avg[1])*(datay[i]-avg[1]);
    }

    dev[0] = sum[0]/num;
    dev[1] = sum[1]/num;
}*/


/*double stat_stdev2D( double *datax, double *datay, int num, double *sdev )
{
    double      dev[2];

    stat_devia2D(datax,datay,num,dev);
    sdev[0] = sqrt(dev[0]);
    sdev[1] = sqrt(dev[1]);
}*/


