import java.lang.*;
import java.util.*;



public class PNeuron extends Object implements Runnable
{
    public double   inp[];
    public ExtSigmoidFunc thrsf;
    public double   wgt[];
    public double   lim;
    public double   out;
    public Thread   runthrd;


/*.....   Constructors   .....*/

    public PNeuron()
    {
        runthrd=null;
        inp=null;  thrsf=null;  out=0.0;
        wgt=null;  lim=0.0;
    }

    public PNeuron( double inparr[], ExtSigmoidFunc thrsfunc, double initwgt, double initlim )
    {
        runthrd=null;
        inp=inparr;
        thrsf=thrsfunc;
        out=0.0;
        lim=initlim;
        wgt=new double[inp.length];
        for ( int i=0; i<wgt.length; i++ )  wgt[i]=initwgt;
    }

    public PNeuron( PNeuron other )
    {
        runthrd=null;
        inp=other.inp;
        thrsf=other.thrsf;
        out=other.out;
        lim=other.lim;
        wgt=new double[other.wgt.length];
        for ( int i=0; i<wgt.length; i++ )  wgt[i]=other.wgt[i];
    }


/*.....   "Object" parent class extensions   .....*/

    protected void finalize() throws Throwable
    {
        thrsf=null;  runthrd=null;
    }

    // public boolean equals( Object anotherObject )    {}

    public String toString()
    {
        int     i=0;
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("{ PNeuron:\n");

        sbuff.append("inputs: [");
        for ( i=0; i<inp.length-1; i++ )  sbuff.append(inp[i]+",");
        sbuff.append(inp[inp.length-1]+"]\n");

        sbuff.append("transfer function: ");
        if (thrsf!=null)  sbuff.append(thrsf.toString()+"\n");
        else  sbuff.append("(null)\n");

        sbuff.append("weights: [");
        for ( i=0; i<wgt.length-1; i++ )  sbuff.append(wgt[i]+",");
        sbuff.append(wgt[wgt.length-1]+"]\n");
        
        sbuff.append("weight bias: "+lim+"\n");
        sbuff.append("output: "+out+" }\n");

        return(sbuff.toString());
    }

    public int hashCode()
    {
        return(super.hashCode()+inp.length+(int)lim+thrsf.hashCode());
    }

    protected Object clone() throws CloneNotSupportedException
    {
        throw(new CloneNotSupportedException());
    }


/*.....   Class-specific functionality   .....*/

    public synchronized void initWeights( double initwgt, double initlim )
    {
        for ( int i=0; i<wgt.length; i++ )
            wgt[i]=initwgt;

        lim=initwgt;
    }
        
    public synchronized double fire()
    {
        int     i=0;
        double  sum=0.0;


        for ( i=0; i<wgt.length; i++ )
            sum = sum + inp[i]*wgt[i];

        sum = sum + lim;

        if (thrsf!=null)  out = thrsf.value(sum);
        else  out = sum;

        // try { Thread.currentThread().sleep(1000); } catch(InterruptedException e) {}
        
        return(out);
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
        int     i=0;
        
        double   inparr[]=new double[10];
        for ( i=0; i<inparr.length; i++ )  inparr[i]=1.0;

        PNeuron  neu=new PNeuron(inparr,new ExtSigmoidFunc(1.0),0.05,1.0);

        System.out.println(neu.toString());

        System.out.println("Firing neuron...");
        neu.fire();
        
        System.out.println(neu.toString());
    }
    
}

        
