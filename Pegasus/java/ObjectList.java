import java.util.*;



public class ObjectList extends Vector
{
    protected boolean       LRFOflag;
    protected int           elemLimit;
    final static boolean    defLRFOflag=true;
    final static int        defElemLimit=-1;

    

    public ObjectList()
    {
        super();
        LRFOflag=defLRFOflag;
        elemLimit=defElemLimit;
    }


    public ObjectList( int initialCapacity )
    {
        super(initialCapacity);
        LRFOflag=defLRFOflag;
        elemLimit=defElemLimit;
    }


    public ObjectList( int initialCapacity, int capacityIncrement )
    {
        super(initialCapacity,capacityIncrement);
        LRFOflag=defLRFOflag;
        elemLimit=defElemLimit;
    }


    public void setLRFO( boolean useLRFO )
    {
        LRFOflag=useLRFO;
    }


    public boolean isLRFO()
    {
        return(LRFOflag);
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
    

    public void put( Object element )
    {
        boolean     exists=contains(element);
        
        if (isBounded() && (occupied()>=limit()) && (!exists))
        {
          /*** safety code, when List is on "hold" and empty (no last) ***/
            try  { removeElement(lastElement()); }
            catch (NoSuchElementException e)  {}
        }
            
        if (limit()!=0)
        {
            if (LRFOflag)
            {
                if (exists)  removeElement(element);
                insertElementAt(element,0);
            }
            else
            {
                if (!exists)  addElement(element);
            }
        }
    }


    public void touch( Object element )
    {
        if (LRFOflag && contains(element))
        {
            removeElement(element);
            insertElementAt(element,0);
        }
    }


    public String ToString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(super.toString());
        sbuff.append("\n::  LRFO="+LRFOflag);
        sbuff.append("  elemLimit="+elemLimit);

        return(sbuff.toString());
    }
        

/************
    public static void main( String args[] )
    {
        ObjectList l=new ObjectList();
    
        l.put(new Integer(1));
        l.put(new Integer(3));
        l.put(new Integer(4));
        l.put(new Integer(2));
        System.out.println(l.toString());
        l.put(new Integer(5));
        System.out.println(l.toString());
        l.put(new Integer(2));
        System.out.println(l.toString());
        l.touch(new Integer(5));
    
        System.out.println(l.toString());
    }
************/

}



