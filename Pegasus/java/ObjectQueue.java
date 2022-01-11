import java.util.*;



public class ObjectQueue extends Vector
{
    protected int       elemLimit;
    final static int    defElemLimit=-1;

    

    public ObjectQueue()
    {
        super();
        elemLimit=defElemLimit;
    }


    public ObjectQueue( int initialCapacity )
    {
        super(initialCapacity);
        elemLimit=defElemLimit;
    }


    public ObjectQueue( int initialCapacity, int capacityIncrement )
    {
        super(initialCapacity,capacityIncrement);
        elemLimit=defElemLimit;
    }


    public void setLimit( int elemlim )
    {
        elemLimit=elemlim;
    }


    public int limit()
    {
        return(elemLimit);
    }


    public void bound( int lim )
    {
        if (lim>0)  setLimit(lim);
    }


    public void unbound()
    {
        setLimit(-1);
    }


    public boolean isBounded()
    {
        return(limit()>0);
    }


    public boolean isFull()
    {
        return(occupied()==limit());
    }

    
    public void hold()
    {
      /*** limit=0 -> do not add any new elements ***/
        setLimit(0);
    }


    public boolean isOnHold()
    {
        return(elemLimit==0);
    }

    
    public int occupied()
    {
        return(elementCount);
    }
    

    public void push( Object element )
    {
        if ( ((!isBounded())||(occupied()<limit()))
                && (!isOnHold()) && (!contains(element)) )
        {
            if (isEmpty())
                addElement(element);
            else
                insertElementAt(element,indexOf(lastElement())+1);
        }
    }


    public Object pop() throws IllegalAccessException
    {
        if (!isEmpty())
        {
            Object obj=elementAt(indexOf(firstElement()));
            removeElementAt(indexOf(firstElement()));
            return(obj);
        }
        else
            throw(new IllegalAccessException("empty"));
    }
    
            
    public Object peek() throws IllegalAccessException
    {
        if (!isEmpty())
        {
            Object obj=elementAt(indexOf(firstElement()));
            return(obj);
        }
        else
            throw(new IllegalAccessException("empty"));
    }


    public String ToString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(super.toString());
        sbuff.append("\n::  elemLimit="+elemLimit);

        return(sbuff.toString());
    }
        

/************
    public static void main( String args[] )
    {
        ObjectQueue q=new ObjectQueue();
    
        q.push(new Integer(1));
        q.push(new Integer(3));
        q.push(new Integer(4));
        q.push(new Integer(2));
        System.out.println(q.toString());
        try
        {
            System.out.println("Pop: "+q.pop().toString());
            System.out.println(q.toString());
            q.push(new Integer(4));
            System.out.println(q.toString());
            System.out.println("Pop: "+q.pop().toString());
        }
        catch (Exception e) {System.out.println("error");}
    
        System.out.println(q.toString());
    }
*************/    

}



