import java.lang.*;
import java.util.*;



public class PNetwork extends Object implements Runnable
{
    public double   inp[];
    public PNeuron  neu[];
    public double   out[];

    public int      fwsize;

    ThreadGroup     rungrp;
    
    public static int   fws_iterative=1;
    public static int   fws_concurrent=Integer.MAX_VALUE;
    public static int   deffwsize=fws_iterative;


/*.....   Constructors   .....*/

    public PNetwork()
    {
        inp=null;  neu=null;  out=null;
        rungrp=null;  fwsize=deffwsize;
    }

    public PNetwork( int inpsz, int outsz, ExtSigmoidFunc neutrf, double initwgt,
                     double initlim, int fwinsize )
    {
        rungrp=new ThreadGroup("fire-window");
        
        if (fwinsize<fws_iterative)  fwsize=deffwsize;
        else  fwsize=fwinsize;
        
        if ((inpsz<=0)||(outsz<=0))
        {
            inp=null;  neu=null;  out=null;
        }
        else
        {
            inp=new double[inpsz];
            out=new double[outsz];

            neu=new PNeuron[outsz];
            for ( int i=0; i<neu.length; i++ )
                neu[i]=new PNeuron(inp,new ExtSigmoidFunc(neutrf.param,neutrf.limit),initwgt,initlim);
        }
    }

    public PNetwork( double inparr[], int outsz, ExtSigmoidFunc neutrf, double initwgt,
                     double initlim, int fwinsize )
    {
        rungrp=new ThreadGroup("fire-window");
        
        if (fwinsize<fws_iterative)  fwsize=deffwsize;
        else  fwsize=fwinsize;
        
        inp=inparr;

        if (outsz<=0)
        {
            neu=null;  out=null;
        }
        else
        {
            out=new double[outsz];

            neu=new PNeuron[outsz];
            for ( int i=0; i<neu.length; i++ )
                neu[i]=new PNeuron(inp,new ExtSigmoidFunc(neutrf.param,neutrf.limit),initwgt,initlim);
        }
    }

    public PNetwork( PNetwork other )
    {
        inp=other.inp;  neu=other.neu;  out=other.out;
        rungrp=other.rungrp;  fwsize=other.fwsize;
    }


/*.....   Extend "Object" method functionality   .....*/

    protected void finalize() throws Throwable
    {
        inp=null;  neu=null;  out=null;
        rungrp=null;
    }

    // public boolean equals( Object anotherObject )    {}

    public String toString()
    {
        int     i=0;
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("{ PNetwork:  (fwsize:"+fwsize+")\n");

        sbuff.append("inputs: [");
        for ( i=0; i<inp.length-1; i++ )  sbuff.append(inp[i]+",");
        sbuff.append(inp[inp.length-1]+"]\n");

        sbuff.append("neurons: [");
        for ( i=0; i<neu.length-1; i++ )  sbuff.append(neu[i]+",");
        sbuff.append(neu[neu.length-1]+"]\n");

        sbuff.append("outputs: [");
        for ( i=0; i<out.length-1; i++ )  sbuff.append(out[i]+",");
        sbuff.append(out[out.length-1]+"]\n");

        sbuff.append("}\n");
        
        return(sbuff.toString());
    }
        
    public int hashCode()
    {
        return(super.hashCode()+inp.length+neu.length+out.length);
    }

    protected Object clone() throws CloneNotSupportedException
    {
        throw(new CloneNotSupportedException());
    }


/*.....   Class-specific functionality   .....*/

    public boolean isActive()
    {
        return((inp!=null)&&(neu!=null)&&(out!=null));
    }

    public boolean isBusy()
    {
        return(rungrp.activeCount()>0);
    }

    protected synchronized void syncNeuronOutput()
    {
        for ( int i=0; i<out.length; i++ )
            out[i]=neu[i].out;
    }

    public synchronized void fire()
    {
        // System.out.println("Firing Network (sync_fire)");
        
        for ( int i=0; i<neu.length; i++ )
        {
            // System.out.println("\tFiring neuron #"+(i+1));
            neu[i].fire();
        }

        syncNeuronOutput();
    }

    public synchronized void multi_fire( int fwinsize )
    {
        // System.out.println("Firing Network (multi_fire)");
        
        if (fwinsize<=0)  return;
        
        for ( int i=0; i<neu.length; i++ )
        {
            while (rungrp.activeCount()>=fwinsize)
                Thread.currentThread().yield();

            // System.out.println("\tFiring neuron #"+(i+1));
            neu[i].thread_fire(rungrp);
        }

        while (rungrp.activeCount()>0)  Thread.currentThread().yield();
        syncNeuronOutput();
    }

    public synchronized void multi_prio_fire( int fwinsize, int prio )
    {
        // System.out.println("Firing Network (multi_prio_fire)");
        
        if (fwinsize<=0)  return;
        
        if ((prio>=Thread.MIN_PRIORITY)&&(prio<=Thread.MAX_PRIORITY))
            rungrp.setMaxPriority(prio);
            
        for ( int i=0; i<neu.length; i++ )
        {
            while (rungrp.activeCount()>=fwinsize)
                Thread.currentThread().yield();

            // System.out.println("\tFiring neuron #"+(i+1));
            neu[i].thread_prio_fire(rungrp,prio);
        }

        while (rungrp.activeCount()>0)  Thread.currentThread().yield();
        syncNeuronOutput();
    }

    public void run()
    {
        if (fwsize==fws_iterative)  fire();
        else  multi_prio_fire(fwsize,Thread.currentThread().getPriority());

        // while (rungrp.activeCount()>0)  Thread.currentThread().yield();
        rungrp.stop();
    }
    

/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        Timer   clock=new Timer();
        long    runtime_ms;
        PNetwork    pnet1, pnet2, pnet3;
        int     inpsz=400, outsz=48;
        ThreadGroup grp=new ThreadGroup("net-group");
        Thread      t1, t2, t3;


        System.out.println("Initializing Networks ["+inpsz+"x"+outsz+"]");

        pnet1=new PNetwork(inpsz,outsz,new ExtSigmoidFunc(),0.05,1.0,fws_iterative);
        t1=new Thread(grp,pnet1);
        pnet2=new PNetwork(pnet1.inp,outsz,new ExtSigmoidFunc(),2.5,1.9,fws_iterative);
        t2=new Thread(grp,pnet2);
        pnet3=new PNetwork(pnet1.inp,outsz,new ExtSigmoidFunc(),22.5,10.9,fws_iterative);
        t3=new Thread(grp,pnet3);
       
        System.out.println("Firing Networks...");

        clock.start();

        t1.start();
        t2.start();
        t3.start();
        while (grp.activeCount()>0)  Thread.currentThread().yield();
        grp.stop();

        clock.stop();

        runtime_ms=clock.time();
        System.out.println("finished in "+runtime_ms+" milliseconds");
    }

}


