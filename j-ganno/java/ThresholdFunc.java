// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class ThresholdFunc extends NumericFunction
{
    public double thrshold;


    public ThresholdFunc( double thval )
    {
        super("Threshold Function");
        thrshold=thval;
    }


    public Object evaluate( Object xval )
    {
        Double  dval=(Double)xval;
        if (dval.doubleValue()<0.0)  return(new Double(0.0));
        else if (dval.doubleValue()>=thrshold)  return(new Double(thrshold));
        else  return(new Double(dval.doubleValue()/thrshold));
    }


    public String toString()
    {
        return(super.toString()+" (threshold="+thrshold+")");
    }
    

    public static void main( String args[] )
    {
        NumericFunction f=new ThresholdFunc(2.0);
        System.out.println(f.toString());
        
        for (double x=-10.0; x<=10.0; x++ )
            System.out.println("\tx = "+x+"    \tf(x) = "+f.value(x));
    }

}


