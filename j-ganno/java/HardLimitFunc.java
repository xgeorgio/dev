// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class HardLimitFunc extends NumericFunction
{
    public HardLimitFunc()
    {
        super("Hard Limit Function");
    }


    public Object evaluate( Object xval )
    {
        Double  dval=(Double)xval;
        if (dval.doubleValue()>=0.0)  return(new Double(1.0));
        else  return(new Double(-1.0));
    }


    public String toString()
    {
        return(super.toString());
    }
    

    public static void main( String args[] )
    {
        NumericFunction f=new HardLimitFunc();
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


