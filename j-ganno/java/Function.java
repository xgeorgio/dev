// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;


public abstract class Function extends Object
{
    protected String    name;
    protected Object    i, o;
    

    
    public Function( String funcname )
    {
        name=new String(funcname);
        i=o=null;
    }


    public abstract Object evaluate( Object ival );


    public Object lookup( Object ival )
    {
        if (ival==null)  return(null);
        else if (ival.equals(i))  return(o);
        else
        {
            i=ival;
            o=evaluate(ival);
            return(o);
        }
    }


    public int hashCode()
    {
        return(name.hashCode());
    }


    public String toString()
    {
        return(name);
    }

}


