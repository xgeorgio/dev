// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;
import java.util.*;



public class Neuron extends Object implements Runnable
{
    public Vector   inp;
    public NumericFunction thrsf;
    public Vector   wgt;
    public Weight   lim;
    public Weight   out;
    public Thread   runthrd;


/*.....   Constructors   .....*/

    public Neuron()
    {
        inp=null;  thrsf=null;  out=null;
        wgt=null;  lim=null;
    }

    public Neuron( Vector inparr, NumericFunction thrsfunc, Weight outvar )
    {
        inp=inparr;
        thrsf=thrsfunc;
        out=outvar;
    }

    public Neuron( Neuron other )
    {
        inp=other.inp;
        thrsf=other.thrsf;
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
        inp=null;  thrsf=null;  wgt=null;
        lim=null;  out=null;  runthrd=null;
    }

    public int hashCode()
    {
        if (isActive())  return(wgt.hashCode()+lim.hashCode());
        else  return(0);
    }

    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("Neuron:\n");
        if (inp!=null)  sbuff.append(inp.toString()+"\n");
        if (thrsf!=null)  sbuff.append(thrsf.toString()+"\n");
        if (wgt!=null)  sbuff.append(wgt.toString()+"\n");
        if (lim!=null)  sbuff.append(lim.toString()+"\n");
        if (out!=null)  sbuff.append(out.toString()+"\n");

        return(sbuff.toString());
    }
        

/*.....   Class-specific functionality   .....*/

    public boolean isActive()
    {
        return((inp!=null)&&(thrsf!=null)&&(wgt!=null)
               &&(lim!=null)&&(out!=null));
    }

    public synchronized void constructWeightLayer( Vector inparr, double initwgt, boolean addinparr )
    {
        Object  cur1, cur2, cur3;
        int     pos1, pos2, pos3;
        Enumeration e1, e2, e3;
        Vector  v1, v2, v3, newwgt;


        if (inp==null)  inp=new Vector();
        if (wgt==null)  wgt=new Vector();
        
        if (inparr==null)  return;

        if (addinparr==true)  inp.addElement(inparr);
        newwgt=new Vector(inparr.size());
        wgt.addElement(newwgt);

/*... lev-1 ...*/
        v1=inparr;
        e1=v1.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=v1.indexOf(cur1);
            if (cur1 instanceof Weight)
                newwgt.insertElementAt(new Weight(initwgt),pos1);
            else if (cur1 instanceof Vector)
            {
                v2=(Vector)cur1;
                newwgt.insertElementAt(new Vector(v2.size()),pos1);

/*... lev-2 ...*/
                e2=v2.elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=v2.indexOf(cur2);
                    if (cur2 instanceof Weight)
                        ((Vector)newwgt.elementAt(pos1)).insertElementAt(new Weight(initwgt),pos2);
                    else if (cur2 instanceof Vector)
                    {
                        v3=(Vector)cur2;
                        ((Vector)newwgt.elementAt(pos1)).insertElementAt(new Vector(v3.size()),pos2);

/*... lev-3 ...*/
                        e3=v3.elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=v3.indexOf(cur3);
                            if (cur3 instanceof Weight)
                                ((Vector)((Vector)newwgt.elementAt(pos1)).elementAt(pos2)).insertElementAt(new Weight(initwgt),pos3);
                        }
                    }
                }
            }
        }

    }
    
    public synchronized void addWeightLayer( Vector inparr, double initwgt )
    {
        constructWeightLayer(inparr,initwgt,true);
    }
    
    public synchronized void createWeights( double initwgt, double initlim )
    {
        Object  cur;
        Enumeration e;


        lim=new Weight(initlim);

        if (inp==null)  return;

        wgt=new Vector();

/*... lev-0 ...*/
        e=inp.elements();
        while (e.hasMoreElements())
        {
            cur=e.nextElement();
            if (cur instanceof Vector)
            {
                // System.out.println("adding weight layer: (inp="+((Vector)cur).toString()+")");
                constructWeightLayer((Vector)cur,initwgt,false);
            }
        }

    }
    
    public synchronized void mirrorWeights( Neuron other )
    {
        wgt=other.wgt;
        lim=other.lim;
    }
    

    public synchronized double fire()
    {
        Object  cur1, cur2, cur3, cur4;
        int     pos1, pos2, pos3, pos4;
        Enumeration e1, e2, e3, e4;
        double  ival, wval;
        double  sum=0.0;

        
/*... lev-0 ...*/
        e1=wgt.elements();
        while (e1.hasMoreElements())
        {
            cur1=e1.nextElement();
            pos1=wgt.indexOf(cur1);
            if (cur1 instanceof Vector)
            {
/*... lev-1 ...*/
                e2=((Vector)cur1).elements();
                while (e2.hasMoreElements())
                {
                    cur2=e2.nextElement();
                    pos2=((Vector)cur1).indexOf(cur2);
                    if (cur2 instanceof Weight)
                    {
                        // System.out.println("Firing weight: (["+pos1+"],"+pos2+")");
                        ival=((Weight)((Vector)inp.elementAt(pos1)).elementAt(pos2)).value();
                        wval=((Weight)((Vector)wgt.elementAt(pos1)).elementAt(pos2)).value();
                        sum = sum + ival*wval;
                    }
                    else if (cur2 instanceof Vector)
                    {
/*... lev-2 ...*/
                        e3=((Vector)cur2).elements();
                        while (e3.hasMoreElements())
                        {
                            cur3=e3.nextElement();
                            pos3=((Vector)cur2).indexOf(cur3);
                            if (cur3 instanceof Weight)
                            {
                                // System.out.println("Firing weight: (["+pos1+"],"+pos2+","+pos3+")");
                                ival=((Weight)((Vector)((Vector)inp.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).value();
                                wval=((Weight)((Vector)((Vector)wgt.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).value();
                                sum = sum + ival*wval;
                            }
                            else if (cur3 instanceof Vector)
                            {
/*... lev-3 ...*/
                                e4=((Vector)cur3).elements();
                                while (e4.hasMoreElements())
                                {
                                    cur4=e4.nextElement();
                                    pos4=((Vector)cur3).indexOf(cur4);
                                    if (cur4 instanceof Weight)
                                    {
                                        // System.out.println("Firing weight: (["+pos1+"],"+pos2+","+pos3+","+pos4+")");
                                        ival=((Weight)((Vector)((Vector)((Vector)inp.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).elementAt(pos4)).value();
                                        wval=((Weight)((Vector)((Vector)((Vector)wgt.elementAt(pos1)).elementAt(pos2)).elementAt(pos3)).elementAt(pos4)).value();
                                        sum = sum + ival*wval;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (lim!=null)  sum = sum + lim.value();

        if (thrsf!=null)  out.setValue(thrsf.value(sum));
        else  out.setValue(sum);

        // try { Thread.currentThread().sleep(5000); } catch(InterruptedException e) {}
        
        return(out.value());
    }

    public void thread_fire( ThreadGroup grp )
    {
        runthrd=new Thread(grp,this);
        runthrd.start();
    }

    public void thread_prio_fire( ThreadGroup grp, int prio )
    {
        runthrd=new Thread(grp,this);
        if ((prio>=Thread.MIN_PRIORITY)&&(prio<=Thread.MAX_PRIORITY))
        {
            if (prio<=grp.getMaxPriority()) runthrd.setPriority(prio);
            else  runthrd.setPriority(grp.getMaxPriority());
        }
        runthrd.start();
    }

    public void run()
    {
        fire();
        if (runthrd!=null)
            { runthrd.stop();  runthrd=null; }
    }
       

/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        Vector inp=new Vector(2), tmp;

        tmp=new Vector(3);
        inp.addElement(tmp);

        tmp.addElement(new Weight(1.1));
        tmp.addElement(new Weight(1.2));
        tmp.addElement(new Weight(1.3));
/*
        ((Vector)inp.elementAt(0)).addElement(new Vector(2));
        ((Vector)inp.elementAt(0)).addElement(new Vector(2));
        ((Vector)inp.elementAt(0)).addElement(new Vector(2));

        ((Vector)((Vector)inp.elementAt(0)).elementAt(0)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(0)).elementAt(0)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(0)).elementAt(1)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(0)).elementAt(1)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(0)).elementAt(2)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(0)).elementAt(2)).addElement(new Vector(2));

        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(0)).elementAt(0)).addElement(new Weight(11.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(0)).elementAt(0)).addElement(new Weight(11.2));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(0)).elementAt(1)).addElement(new Weight(12.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(0)).elementAt(1)).addElement(new Weight(12.2));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(1)).elementAt(0)).addElement(new Weight(21.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(1)).elementAt(0)).addElement(new Weight(21.2));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(1)).elementAt(1)).addElement(new Weight(22.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(1)).elementAt(1)).addElement(new Weight(22.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(2)).elementAt(0)).addElement(new Weight(31.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(2)).elementAt(0)).addElement(new Weight(31.2));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(2)).elementAt(1)).addElement(new Weight(32.1));
        ((Vector)((Vector)((Vector)inp.elementAt(0)).elementAt(2)).elementAt(1)).addElement(new Weight(32.2));
*/
        tmp=new Vector(3);
        inp.addElement(tmp);

        tmp.addElement(new Weight(2.1));
        tmp.addElement(new Weight(2.2));
        tmp.addElement(new Weight(2.3));
/*
        ((Vector)inp.elementAt(1)).addElement(new Vector(2));
        ((Vector)inp.elementAt(1)).addElement(new Vector(2));
        ((Vector)inp.elementAt(1)).addElement(new Vector(2));

        ((Vector)((Vector)inp.elementAt(1)).elementAt(0)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(1)).elementAt(0)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(1)).elementAt(1)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(1)).elementAt(1)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(1)).elementAt(2)).addElement(new Vector(2));
        ((Vector)((Vector)inp.elementAt(1)).elementAt(2)).addElement(new Vector(2));

        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(0)).elementAt(0)).addElement(new Weight(11.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(0)).elementAt(0)).addElement(new Weight(11.2));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(0)).elementAt(1)).addElement(new Weight(12.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(0)).elementAt(1)).addElement(new Weight(12.2));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(1)).elementAt(0)).addElement(new Weight(21.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(1)).elementAt(0)).addElement(new Weight(21.2));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(1)).elementAt(1)).addElement(new Weight(22.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(1)).elementAt(1)).addElement(new Weight(22.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(2)).elementAt(0)).addElement(new Weight(31.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(2)).elementAt(0)).addElement(new Weight(31.2));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(2)).elementAt(1)).addElement(new Weight(32.1));
        ((Vector)((Vector)((Vector)inp.elementAt(1)).elementAt(2)).elementAt(1)).addElement(new Weight(32.2));
*/

        Neuron neu=new Neuron(inp,null,new Weight(0.0));
        neu.createWeights(1.0,1.4);
        ((Weight)((Vector)neu.wgt.elementAt(1)).elementAt(0)).setValue(5.0);
        neu.addWeightLayer((Vector)inp.elementAt(0),0.25);

//        Neuron neu=new Neuron(null,new SigmoidFunc(1.0),new Weight(0.0));
//        neu.addWeightLayer((Vector)inp.elementAt(0),1.0);
//        neu.addWeightLayer((Vector)inp.elementAt(1),2.0);

        System.out.println(neu.toString());

        ThreadGroup g=new ThreadGroup("group");
        // Neuron neu1=new Neuron(neu);
        // neu1.mirrorWeights(neu);

        // System.out.println("firing neuron #1 (threads="+g.activeCount()+")");
        neu.fire();
        // while (g.activeCount()>0)  System.out.println("waiting...");
        // System.out.println("firing neuron #2 (threads="+g.activeCount()+")");
        // neu1.thread_prio_fire(g,Thread.NORM_PRIORITY);
        // System.out.println("neuron #2 fired (threads="+g.activeCount()+")");

        System.out.println(neu.toString());

    }
    
}

        
