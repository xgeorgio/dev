// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class ExtHardLimitFunc extends NumericFunction
{
    public double limit;


    public ExtHardLimitFunc( double limval )
    {
        super("Extended Hard Limit Function");
        limit=limval;
    }


    public Object evaluate( Object xval )
    {
        Double  dval=(Double)xval;
        if (dval.doubleValue() < -limit)  return(new Double(-1.0));
        else if (dval.doubleValue() > limit)  return(new Double(1.0));
        else  return(new Double(0.0));
    }


    public String toString()
    {
        return(super.toString()+" (limit="+limit+")");
    }
    

    public static void main( String args[] )
    {
        NumericFunction f=new ExtHardLimitFunc(0.25);
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


