// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class HyperTanFunc extends NumericFunction
{
    public HyperTanFunc()
    {
        super("Hyperbolic Tangent Function");
    }


    public Object evaluate( Object xval )
    {
        Double  dval=(Double)xval;
        double  ex=Math.exp(dval.doubleValue());
        return(new Double((ex - 1/ex)/(ex + 1/ex)));
    }


    public String toString()
    {
        return(super.toString());
    }
    

    public static void main( String args[] )
    {
        NumericFunction f=new HyperTanFunc();
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


