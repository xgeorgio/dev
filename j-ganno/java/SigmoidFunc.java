// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class SigmoidFunc extends NumericFunction
{
    public double sparam;


    public SigmoidFunc( double spval )
    {
        super("Simoid Function");
        sparam=spval;
    }


    public Object evaluate( Object xval )
    {
        Double  dval=(Double)xval;
        return(new Double(1.0/(1+Math.exp(-sparam*dval.doubleValue()))));
    }


    public String toString()
    {
        return(super.toString()+" (factor="+sparam+")");
    }
    

    public static void main( String args[] )
    {
        NumericFunction f=new SigmoidFunc(1.0);
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


