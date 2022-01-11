// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;
import java.util.*;


public class Layer extends Object
{
    public Vector inp;
    public Vector neu;
    public Vector out;



/*.....   Constructors   .....*/

    public Layer()
    {
        inp=null;  neu=null;  out=null;
    }

    public Layer( Vector inparr, Vector outarr )
    {
        inp=inparr;
        out=outarr;
    }

    public Layer( Layer other )
    {
        inp=other.inp;
        out=other.out;
    }


/*.....   Extend "Object" method functionality   .....*/

    protected Object clone() throws CloneNotSupportedException
    {
        throw(new CloneNotSupportedException());
    }

    // public boolean equals( Object anotherObject )
    // {
    //     return(super.equals(anotherObject));
    // }

    protected void finalize() throws Throwable
    {
        inp=null;  neu=null;  out=null;
    }

    public int hashCode()
    {
        if (isActive())  return(inp.hashCode()+neu.hashCode()+out.hashCode());
        else  return(0);
    }

    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("Layer:\n");
        if (inp!=null)  sbuff.append(inp.toString()+"\n");
        if (neu!=null)  sbuff.append(neu.toString()+"\n");
        if (out!=null)  sbuff.append(out.toString()+"\n");

        return(sbuff.toString());
    }
        

/*.....   Class-specific functionality   .....*/

    public boolean isActive()
    {
        return((inp!=null)&&(neu!=null)&&(out!=null));
    }

    public synchronized void createNeurons( NumericFunction thrsfunc, double initwgt, double initlim )
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;
        Vector  v1, v2, v3;
        Neuron  newneu;


/*... lev-1 ...*/
        v1=out;
        neu=new Vector(v1.size());
        
        e1=v1.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=v1.indexOf(cur1);
            if (cur1 instanceof Weight)
            {
                neu.insertElementAt(new Neuron(inp,thrsfunc,(Weight)out.elementAt(pos1)),pos1);
                ((Neuron)neu.elementAt(pos1)).createWeights(initwgt,initlim);
            }
            else if (cur1 instanceof Vector)
            {
                v2=(Vector)cur1;
                neu.insertElementAt(new Vector(v2.size()),pos1);

/*... lev-2 ...*/
                e2=v2.elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=v2.indexOf(cur2);
                    if (cur2 instanceof Weight)
                    {
                        ((Vector)neu.elementAt(pos1)).insertElementAt(new Neuron(inp,thrsfunc,(Weight)((Vector)out.elementAt(pos1)).elementAt(pos2)),pos2);
                        ((Neuron)((Vector)neu.elementAt(pos1)).elementAt(pos2)).createWeights(initwgt,initlim);
                    }
                    else if (cur2 instanceof Vector)
                    {
                        v3=(Vector)cur2;
                        ((Vector)neu.elementAt(pos1)).insertElementAt(new Vector(v3.size()),pos2);
                        
/*... lev-3 ...*/
                        e3=v3.elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=v3.indexOf(cur3);
                            if (cur3 instanceof Weight)
                            {
                                ((Vector)((Vector)neu.elementAt(pos1)).elementAt(pos2)).insertElementAt(new Neuron(inp,thrsfunc,(Weight)((Vector)((Vector)out.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)),pos3);
                                ((Neuron)((Vector)((Vector)neu.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).createWeights(initwgt,initlim);
                            }
                        }
                    }
                }
            }
        }

    }

    public synchronized void addInputFeed( Vector newinp, double initwgt )
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;
        Vector  v1, v2, v3;
        boolean addinparr=true;


/*... lev-1 ...*/
        v1=neu;
        
        e1=v1.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=v1.indexOf(cur1);
            if (cur1 instanceof Neuron)
            {
                ((Neuron)cur1).constructWeightLayer(newinp,initwgt,addinparr);
                if (addinparr)  addinparr=false;
            }
            else if (cur1 instanceof Vector)
            {
/*... lev-2 ...*/
                v2=(Vector)cur1;

                e2=v2.elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=v2.indexOf(cur2);
                    if (cur2 instanceof Neuron)
                    {
                        ((Neuron)cur2).constructWeightLayer(newinp,initwgt,addinparr);
                        if (addinparr)  addinparr=false;
                    }
                    else if (cur2 instanceof Vector)
                    {
/*... lev-3 ...*/
                        v3=(Vector)cur2;

                        e3=v3.elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=v3.indexOf(cur3);
                            if (cur3 instanceof Neuron)
                            {
                                ((Neuron)cur3).constructWeightLayer(newinp,initwgt,addinparr);
                                if (addinparr)  addinparr=false;
                            }
                        }
                    }
                }
            }
        }

    }
    
    public synchronized void mirrorNeurons( Layer other )
    {
        neu=other.neu;
    }
    
    public synchronized void fire()
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;

        
/*... lev-1 ...*/
        e1=neu.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=neu.indexOf(cur1);
            if (cur1 instanceof Neuron)
            {
                // System.out.println("Firing neuron: ("+pos1+")");
                ((Weight)out.elementAt(pos1)).setValue(((Neuron)cur1).fire());
            }
            else if (cur1 instanceof Vector)
            {
/*... lev-2 ...*/
                e2=((Vector)cur1).elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=((Vector)cur1).indexOf(cur2);
                    if (cur2 instanceof Neuron)
                    {
                        // System.out.println("Firing neuron: ("+pos1+","+pos2+")");
                        ((Weight)((Vector)out.elementAt(pos1)).elementAt(pos2)).setValue(((Neuron)cur2).fire());
                    }
                    else if (cur2 instanceof Vector)
                    {
/*... lev-3 ...*/
                        e3=((Vector)cur2).elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=((Vector)cur2).indexOf(cur3);
                            if (cur3 instanceof Neuron)
                            {
                                // System.out.println("Firing neuron: ("+pos1+","+pos2+","+pos3+")");
                                ((Weight)((Vector)((Vector)out.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).setValue(((Neuron)cur3).fire());
                            }
                        }
                    }
                }
            }
        }

    }

    public synchronized void group_fire( int fwinsize )
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;
        ThreadGroup  tgrp=new ThreadGroup("fire-window");


        if (fwinsize<=0)  return;
        
/*... lev-1 ...*/
        e1=neu.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=neu.indexOf(cur1);
            if (cur1 instanceof Neuron)
            {
                while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                // System.out.println("Firing neuron: ("+pos1+")");
                ((Neuron)cur1).thread_fire(tgrp);
            }
            else if (cur1 instanceof Vector)
            {
/*... lev-2 ...*/
                e2=((Vector)cur1).elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=((Vector)cur1).indexOf(cur2);
                    if (cur2 instanceof Neuron)
                    {
                        while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                        // System.out.println("Firing neuron: ("+pos1+","+pos2+")");
                        ((Neuron)cur2).thread_fire(tgrp);
                    }
                    else if (cur2 instanceof Vector)
                    {
/*... lev-3 ...*/
                        e3=((Vector)cur2).elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=((Vector)cur2).indexOf(cur3);
                            if (cur3 instanceof Neuron)
                            {
                                while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                                // System.out.println("Firing neuron: ("+pos1+","+pos2+","+pos3+")");
                                ((Neuron)cur3).thread_fire(tgrp);
                            }
                        }
                    }
                }
            }
        }

    }

    public synchronized void group_prio_fire( int fwinsize, int prio )
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;
        ThreadGroup  tgrp=new ThreadGroup("fire-window");


        if (fwinsize<=0)  return;
        
        if ((prio>=Thread.MIN_PRIORITY)&&(prio<=Thread.MAX_PRIORITY))
            tgrp.setMaxPriority(prio);
        
/*... lev-1 ...*/
        e1=neu.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=neu.indexOf(cur1);
            if (cur1 instanceof Neuron)
            {
                while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                // System.out.println("Firing neuron: ("+pos1+")");
                ((Neuron)cur1).thread_prio_fire(tgrp,tgrp.getMaxPriority());
            }
            else if (cur1 instanceof Vector)
            {
/*... lev-2 ...*/
                e2=((Vector)cur1).elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=((Vector)cur1).indexOf(cur2);
                    if (cur2 instanceof Neuron)
                    {
                        while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                        // System.out.println("Firing neuron: ("+pos1+","+pos2+")");
                        ((Neuron)cur2).thread_prio_fire(tgrp,tgrp.getMaxPriority());
                    }
                    else if (cur2 instanceof Vector)
                    {
/*... lev-3 ...*/
                        e3=((Vector)cur2).elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=((Vector)cur2).indexOf(cur3);
                            if (cur3 instanceof Neuron)
                            {
                                while (tgrp.activeCount()>=fwinsize)  Thread.currentThread().yield();
                                // System.out.println("Firing neuron: ("+pos1+","+pos2+","+pos3+")");
                                ((Neuron)cur3).thread_prio_fire(tgrp,tgrp.getMaxPriority());
                            }
                        }
                    }
                }
            }
        }

    }
          

/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        Vector inpv=new Vector();
        
        Vector inp=new Vector(3);
        inp.addElement(new Weight(1.0));
        inp.addElement(new Weight(2.0));
        inp.addElement(new Weight(4.0));

        inpv.addElement(inp);

        Vector outv=new Vector(2);
        outv.addElement(new Weight(0.1));
        outv.addElement(new Weight(0.1));
/*
        Vector outv=new Vector(2);
        outv.addElement(new Weight(0.1));
        outv.addElement(new Vector(2));
        ((Vector)outv.elementAt(1)).addElement(new Weight(1.1));
        ((Vector)outv.elementAt(1)).addElement(new Vector(2));
        ((Vector)((Vector)outv.elementAt(1)).elementAt(1)).addElement(new Weight(2.1));
        ((Vector)((Vector)outv.elementAt(1)).elementAt(1)).addElement(new Weight(2.2));
*/        
        
        Layer lay=new Layer(inpv,outv);
        lay.createNeurons(null,0.25,1.0);
        lay.addInputFeed((Vector)inpv.elementAt(0),0.3);
        lay.addInputFeed(lay.out,0.5);

        ((Weight)((Vector)lay.inp.elementAt(0)).elementAt(0)).setValue(21.0);

        lay.fire();
        // lay.group_fire(4);

        System.out.println(lay.toString());
    }  

}

    
