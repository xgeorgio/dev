// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public class Weight extends Object
{
    public Double   w;


/*.....   Constructors   .....*/

    public Weight()
    {
        w=null;
    }

    public Weight( double initval )
    {
        w=new Double(initval);
    }

    public Weight( Weight wvar )
    {
        w=wvar.w;
    }


/*.....   Extend "Object" method functionality   .....*/

    protected Object clone() throws CloneNotSupportedException
    {
        Weight  wvar;
        if (isActive()) wvar=new Weight(value());
        else  wvar=new Weight();
        return(wvar);
    }
/*****
    public boolean equals( Object anotherObject )
    {
        if ( (anotherObject!=null)&&(anotherObject instanceof Weight) )
            return( value()==((Weight)anotherObject).value() );
        else  return(false);
    }
*****/

    // public boolean equals( Object anotherObject )
    // {
    //     return(super.equals(anotherObject));
    // }

    protected void finalize() throws Throwable
    {
        w=null;
    }

    public int hashCode()
    {
        if (isActive())  return(w.hashCode());
        else  return(0);
    }

    public String toString()
    {
        if (isActive())  return(new String(w.toString()));
        else  return(new String("null"));
    }

    
/*.....   Class-specific functionality   .....*/

    public boolean isActive()
    {
        return(w!=null);
    }

    public Weight copy()
    {
        Weight  wvar;
        if (isActive()) wvar=new Weight(w.doubleValue());
        else  wvar=new Weight();
        return(wvar);
    }

    public double value()
    {
        return(w.doubleValue());
    }

    public synchronized void setValue( double newval )
    {
        w=new Double(newval);
    }


/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        Weight   a1, a2;

        System.out.println("---(a1=null , a2=1.0)---");
        a1=new Weight();
        a2=new Weight(1.0);

        System.out.println("a1: "+a1.toString()+" --> is active? "+a1.isActive());

        System.out.println("---(a1=a2.copy())---");
        a1=a2.copy();
        System.out.println("a1.value() --> "+a1.value());
        System.out.println("a1: "+a1.toString()+" --> is active? "+a1.isActive());
        System.out.println("a1==a2 --> is it? "+(a1==a2));
        System.out.println("a1.equals(a2) --> is it? "+a1.equals(a2));

        System.out.println("---(a1=a2)---");
        a1=a2;
        System.out.println("a1.value() --> "+a1.value());
        System.out.println("a1: "+a1.toString()+" --> is active? "+a1.isActive());
        System.out.println("a1==a2 --> is it? "+(a1==a2));
        System.out.println("a1.equals(a2) --> is it? "+a1.equals(a2));

        System.out.println("---(a2=2.0)---");
        a2.setValue(2.0);
        System.out.println("a1.value() --> "+a1.value());
        System.out.println("a1: "+a1.toString()+" --> is active? "+a1.isActive());
        System.out.println("a1==a2 --> is it? "+(a1==a2));
        System.out.println("a1.equals(a2) --> is it? "+a1.equals(a2));
    }
    
}
    
