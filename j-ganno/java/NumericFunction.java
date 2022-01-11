// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public abstract class NumericFunction extends Function
{
    protected double    x, y;


    
    public NumericFunction( String funcname )
    {
        super(funcname);
    }


    public double value( double xval )
    {
        if (xval!=x)
        {
            x=xval;
            y=((Double)lookup(new Double(x))).doubleValue();
        }

        return(y);
    }

}


