import java.lang.*;


public class ExtSigmoidFunc extends Object
{
    public double param;
    public double limit;
    public double x, y;

    public static double defparam=1.0;
    public static double deflimit=15.0;


/*.....   Constructors   .....*/

    public ExtSigmoidFunc()
    {
        param=defparam;
        limit=deflimit;
    }

    public ExtSigmoidFunc( double sparam )
    {
        param=sparam;
        limit=deflimit;
    }

    public ExtSigmoidFunc( double sparam, double slimit )
    {
        param=sparam;
        limit=slimit;
    }


/*.....   "Object" parent class extensions   .....*/

    // protected void finalize() throws Throwable       {}

    // public boolean equals( Object anotherObject )     {}

    public String toString()
    {
        return("Extended Sigmoid Function (param="+param+" , limit="+limit+")");
    }

    public int hashCode()
    {
        return(super.hashCode()+(int)(param+limit));
    }

    protected Object clone() throws CloneNotSupportedException
    {
        return(new ExtSigmoidFunc(param,limit));
    }


/*.....   Class-specific methods   .....*/

// Note: if this method is to be used by concurrent neurons, it is
//       best that each neuron has its own copy of the function,
//       instead of using a "synchronized" method which may invalidate
//       the concurrency of the execution between the neurons.

    public double value( double xval )
    {
        if (xval!=x)
        {
            x=xval;

            if ((limit>0)&&(x<-limit))
                y=0.0;
            else if ((limit>0)&&(x>limit))
                y=1.0;
            else  /* limit<=0 or -limit<=x<=+limit */
                y=1.0/(1.0+Math.exp(-param*x));
        }

        return(y);
    }


/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        ExtSigmoidFunc f=new ExtSigmoidFunc(1.0,5.0);
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


