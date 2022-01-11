// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;
import java.util.*;


public class MappingFunc extends Function
{
    public Hashtable    map;


    public MappingFunc()
    {
        super("Mapping Function");
        map=new Hashtable();
    }

    public MappingFunc( int size )
    {
        super("Mapping Function");
        map=new Hashtable(size);
    }

    public MappingFunc( Hashtable newmap )
    {
        super("Mapping Function");
        map=newmap;
    }


    public Object evaluate( Object xval )
    {
        Object  res;
        if (map.containsKey(xval))  res=map.get(xval);
        else  res=null; 
        return(res);
    }
    
    public String toString()
    {
        return(super.toString()+"\nMapping:\n"+map.toString());
    }
    

    public static void main( String args[] )
    {
        Object  res;
        Function f=new MappingFunc(2);
        ((MappingFunc)f).map.put("1","A");
        ((MappingFunc)f).map.put("2","B");

        System.out.println(f.toString());
        
        for (int x=1; x<=3; x++ )
        {
            System.out.print("\tx = "+x+"    \tf(x) = ");
            res=f.lookup(""+x);
            if (res!=null)  System.out.println(res);
            else  System.out.println("(not found)");
        }
    }

}


