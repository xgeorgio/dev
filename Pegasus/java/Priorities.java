import java.lang.*;
import java.util.*;


public class Priorities extends Vector
{
    public Priorities()
    {
        super();
    }

    public Priorities( int initialCapacity )
    {
        super(initialCapacity);
    }

    public Priorities( int initialCapacity, int capacityIncrement )
    {
        super(initialCapacity,capacityIncrement);
    }

    public int initPriorities()
    {
        addSorted(Thread.MIN_PRIORITY);
        addSorted(Thread.NORM_PRIORITY);
        addSorted(Thread.MAX_PRIORITY);

        return(3);
    }

    public int addSorted( int newpriority )
    {
        int     inspos=0;
        Enumeration enum=elements();
        Integer     curValue, newPrio=new Integer(newpriority);
        boolean     inserted;


        if (isEmpty())
        {
            insertElementAt(newPrio,0);
            inserted=true;
        }
        else if (!contains(newPrio))
        {
            inserted=false;
            while(enum.hasMoreElements())
            {
                curValue=(Integer)enum.nextElement();
                inspos=indexOf(curValue);
                if (newPrio.intValue() >= curValue.intValue())
                {
                  /*** Insert BEFORE indicated value ***/
                    insertElementAt(newPrio,inspos);
                    inserted=true; break;
                }
            }

            if (!inserted)
            {
                inspos=indexOf(lastElement())+1;
                insertElementAt(newPrio,inspos);
                inserted=true;
            }
        }
        else
            inserted=false;
        
        if (inserted)  trimToSize();

        return(indexOf(newPrio));
    }


    public String ToString()
    {
        return("[ Priorities:  "+super.toString()+"  ]\n");
    }
    

/***************
    public static void main( String args[] )
    {
        Priorities p=new Priorities();
    
        p.initPriorities();
        p.addSorted(-1);
        p.addSorted(0);
        p.addSorted(-5);
        p.addSorted(-5);
        System.out.println(p.toString());
    }
****************/

}




        
