import java.lang.*;
import java.util.*;


public class ObjectPQueue
{
    protected Hashtable     queues;
    protected Priorities    priors;
    protected int           totalLimit;
    public    int           defPriority;


    public ObjectPQueue( int prioArr[], int totalim, int defprio )
    {
        Enumeration enum;
        Integer     curPrio, defp=new Integer(defprio);
        ObjectQueue  curQueue;
        
        
        if (prioArr.length>0)
        {
            priors=new Priorities(prioArr.length);
            for ( int i=0; i<prioArr.length; i++ )
                priors.addSorted(prioArr[i]);
        }
        else
        {
            priors=new Priorities();
            priors.initPriorities();
        }

        totalLimit=totalim;
        defPriority=defp.intValue();
        if (!priors.contains(defp))
            priors.addSorted(defprio);
        
        queues=new Hashtable();
        enum=priors.elements();
        while (enum.hasMoreElements())
        {
            curPrio=new Integer(((Integer)enum.nextElement()).intValue());
            curQueue=new ObjectQueue();
            curQueue.unbound();
            
            queues.put(curPrio,curQueue);
        }
    }


    public ObjectPQueue()
    {
        ObjectQueue  curQueue;
        int     pArr[]=new int[3];


        pArr[0]=Thread.MIN_PRIORITY;
        pArr[1]=Thread.NORM_PRIORITY;
        pArr[2]=Thread.MAX_PRIORITY;
        
        priors=new Priorities(pArr.length);
        queues=new Hashtable();
        for ( int i=0; i<pArr.length; i++ )
        {
            priors.addSorted(pArr[i]);
            curQueue=new ObjectQueue();
            curQueue.unbound();
            
            queues.put(new Integer(pArr[i]),curQueue);
        }
            
        totalLimit=-1;
        defPriority=Thread.NORM_PRIORITY;
    }


    public void setLimit( int totalim )
    {
        totalLimit=totalim;
    }


    public int limit()
    {
        return(totalLimit);
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
        return(totalLimit==0);
    }


    public int occupied()
    {
        Enumeration enum;
        ObjectQueue curQueue;
        int     count;


        enum=queues.elements();
        count=0;
        while(enum.hasMoreElements())
        {
            curQueue=(ObjectQueue)enum.nextElement();
            count=count+curQueue.occupied();
        }

        return(count);
    }


    public boolean isEmpty()
    {
        return(occupied()==0);
    }


    public String toString()
    {
        Enumeration enum;
        Integer     curPrio;
        ObjectQueue curQueue;
        StringBuffer sbuff=new StringBuffer();


        sbuff.append("[  PriorityQueue:\n"+priors.ToString()+"\n");
        enum=queues.keys();
        while(enum.hasMoreElements())
        {
            curPrio=(Integer)enum.nextElement();
            curQueue=(ObjectQueue)queues.get(curPrio);
            sbuff.append("[ priority="+curPrio+"\n");
            sbuff.append(curQueue.ToString());
            sbuff.append("]\n");
        }
        sbuff.append("\n]");

        return(sbuff.toString());
    }


    public void addPriority( int newpriority )
    {
        Integer newPrio=new Integer(newpriority);


        if (!priors.contains(newPrio))
        {
            if (!queues.containsKey(newPrio))
            {
                ObjectQueue newQueue=new ObjectQueue();
                priors.addSorted(newPrio.intValue());
                queues.put(newPrio,newQueue);
            }
        }
    }
        

    public boolean contains( Object element, int objprio )
    {
        Integer     curPrio=new Integer(objprio);
        
        if ( (priors.contains(curPrio))&&(queues.containsKey(curPrio)) )
            return( ((ObjectQueue)queues.get(curPrio)).contains(element) );
        else
            return(false);
    }


    public boolean contains( Object element )
    {
        Enumeration enum;
        boolean     found;


        enum=queues.keys();
        found=false;
        while ((!found)&&(enum.hasMoreElements()))
            found=contains(element,((Integer)enum.nextElement()).intValue());

        return(found);
    }
    
        
    public int priorityOf( Object element ) throws IllegalAccessException
    {
        Enumeration enum;
        Integer     prio;


        enum=queues.keys();
        while (enum.hasMoreElements())
        {
            prio=(Integer)enum.nextElement();
            if (contains(element,prio.intValue()))
                return(prio.intValue());
        }

        throw(new IllegalAccessException("not found"));
    }


    public boolean setPriority( Object element, int newpriority )
    {
        Integer     curPrio, newPrio;
        ObjectQueue curQueue;


        try
        {
            newPrio=new Integer(newpriority);
            curPrio=new Integer(priorityOf(element));

            curQueue=(ObjectQueue)queues.get(curPrio);
            curQueue.removeElement(element);
            curQueue=(ObjectQueue)queues.get(newPrio);
            curQueue.push(element);

            return(true);
        }
        catch (Exception e)
          { return(false); }
    }
        

    public void push( Object element, int setprio )
    {
        if ( ((!isBounded())||(occupied()<limit()))
                && (!isOnHold()) && (!contains(element)) )
        {
            Integer setPrio=new Integer(setprio);
            if (queues.containsKey(setPrio))
                ((ObjectQueue)queues.get(setPrio)).push(element);
        }
    }


    public Object pop() throws IllegalAccessException
    {
        Integer     maxPrio;
        ObjectQueue curQueue;
        Enumeration enum;

        
        if (!isEmpty())
        {
            enum=priors.elements();
            while (enum.hasMoreElements())
            {
                maxPrio=(Integer)enum.nextElement();
                if (queues.containsKey(maxPrio))
                {
                    curQueue=(ObjectQueue)queues.get(maxPrio);
                    if (!curQueue.isEmpty())
                        return(curQueue.pop());
                }
            }
        }

      /*** catches any other case (not found or all empty) ***/
        throw (new IllegalAccessException("empty"));
    }
    
            
    public Object peek() throws IllegalAccessException
    {
        Integer     maxPrio;
        ObjectQueue curQueue;
        Enumeration enum;

        
        if (!isEmpty())
        {
            enum=priors.elements();
            while (enum.hasMoreElements())
            {
                maxPrio=(Integer)enum.nextElement();
                if (queues.containsKey(maxPrio))
                {
                  /*** if the selected queue is empty, exception is genereted ***/
                    return( ((ObjectQueue)queues.get(maxPrio)).peek() );
                }
            }
        }

      /*** catches any other case (not found or all empty) ***/
        throw (new IllegalAccessException("empty"));
    }


/************
    public static void main( String args[] )
    {
        ObjectPQueue q=new ObjectPQueue();
    
        q.push(new Integer(Thread.MIN_PRIORITY+10),  Thread.NORM_PRIORITY);
        q.push(new Integer(Thread.MIN_PRIORITY+100), Thread.MAX_PRIORITY);
        q.push(new Integer(Thread.MIN_PRIORITY),     Thread.MIN_PRIORITY);
        q.push(new Integer(Thread.NORM_PRIORITY),    Thread.MIN_PRIORITY);
        q.push(new Integer(Thread.NORM_PRIORITY+100),Thread.MAX_PRIORITY);
        q.push(new Integer(Thread.NORM_PRIORITY+10), Thread.NORM_PRIORITY);
        q.push(new Integer(Thread.MAX_PRIORITY+100), Thread.MAX_PRIORITY);
        q.push(new Integer(Thread.MAX_PRIORITY+10),  Thread.NORM_PRIORITY);
        q.push(new Integer(Thread.MAX_PRIORITY),     Thread.MIN_PRIORITY);

        System.out.println(q.toString());

        try
        {
            System.out.println("Pop sequence:");
            while (!q.isEmpty())
                System.out.print(((Integer)q.pop()).toString()+"  ");
            System.out.println();
        }
        catch (Exception e)
          { e.printStackTrace(); }
    }
*************/    
    
}
    
