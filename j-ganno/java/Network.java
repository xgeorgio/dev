// -------------------------------------------------
//
//
// -------------------------------------------------
//                         Harris Georgiou, 1998.
// -------------------------------------------------



import java.lang.*;
import java.util.*;



public class Network extends Object
{
    public Vector   inp;
    public Function itransf;
    public Vector   lay;
    public Function otransf;
    public Vector   out;
    public Hashtable params;



/*.....   Constructors   .....*/

    public Network()
    {
        inp=null;  lay=null;  out=null;
        itransf=null;  otransf=null;  params=null;
    }

    public Network( Vector inparr, Vector layarr, Vector outarr,
                    Function inpf, Function outf, Hashtable partbl )
    {
        inp=inparr;  lay=layarr;  out=outarr;
        itransf=inpf;  otransf=outf;  params=partbl;
    }

    public Network( Network other )
    {
        inp=other.inp;  lay=other.lay;  out=other.out;
        itransf=other.itransf;  otransf=other.otransf;  params=other.params;
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
        inp=null;  lay=null;  out=null;
        itransf=null;  otransf=null;  params=null;
    }

    public int hashCode()
    {
        if (isActive())  return(lay.hashCode()+params.hashCode());
        else  return(0);
    }

    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("Generic Network:\n");
        if (itransf!=null)  sbuff.append(itransf.toString()+"\n");
        if (inp!=null)  sbuff.append(inp.toString()+"\n");
        if (lay!=null)  sbuff.append(lay.toString()+"\n");
        if (out!=null)  sbuff.append(out.toString()+"\n");
        if (otransf!=null)  sbuff.append(otransf.toString()+"\n");
        if (params!=null)  sbuff.append(params.toString()+"\n");

        return(sbuff.toString());
    }
        

/*.....   Class-specific functionality   .....*/

    public boolean isActive()
    {
        return((inp!=null)&&(lay!=null)&&(out!=null));
    }

    public synchronized boolean createLayers( int netdescr[][], NumericFunction thrsfunc,
                                            double initwgt, double initlim, double initinp, double initout )
    {
        int     lays, dim, sz, ilay, olay;
        int     cl, cx, cy, cz, x=0, y=1, z=2;
        Vector  cvinp, tmpinp, tmpout;
        Layer   clay;

        
        if (netdescr.length<1)  return(false);

        ilay=0;  olay=netdescr.length-1;
        lays=netdescr.length;
        

/*... setup input array ...*/

        inp=new Vector();
        
        if (netdescr[ilay][x]>0)
        {
            tmpinp=new Vector(netdescr[ilay][x]);
            for ( cx=0; cx<netdescr[ilay][x]; cx++ )
            {
                if (netdescr[ilay][y]<=0)
                    tmpinp.addElement(new Weight(initinp));
                else
                {
                    tmpinp.addElement(new Vector(netdescr[ilay][y]));
                    for ( cy=0; cy<netdescr[ilay][y]; cy++ )
                    {
                        if (netdescr[ilay][z]<=0)
                            ((Vector)tmpinp.elementAt(cx)).addElement(new Weight(initinp));
                        else
                        {
                            ((Vector)tmpinp.elementAt(cx)).addElement(new Vector(netdescr[ilay][z]));
                            for ( cz=0; cz<netdescr[ilay][z]; cz++ )
                                ((Vector)((Vector)tmpinp.elementAt(cx)).elementAt(cy)).addElement(new Weight(initinp));
                        }
                    }
                }
            }

            inp.addElement(tmpinp);
        }


/*... setup output array ...*/

        out=new Vector();
        
        if (netdescr[olay][x]>0)
        {
            tmpout=new Vector(netdescr[olay][x]);
            for ( cx=0; cx<netdescr[olay][x]; cx++ )
            {
                if (netdescr[olay][y]<=0)
                    tmpout.addElement(new Weight(initout));
                else
                {
                    tmpout.addElement(new Vector(netdescr[olay][y]));
                    for ( cy=0; cy<netdescr[olay][y]; cy++ )
                    {
                        if (netdescr[olay][z]<=0)
                            ((Vector)tmpout.elementAt(cx)).addElement(new Weight(initout));
                        else
                        {
                            ((Vector)tmpout.elementAt(cx)).addElement(new Vector(netdescr[olay][z]));
                            for ( cz=0; cz<netdescr[olay][z]; cz++ )
                                ((Vector)((Vector)tmpout.elementAt(cx)).elementAt(cy)).addElement(new Weight(initout));
                        }
                    }
                }
            }

            out.addElement(tmpout);
        }


/*... setup layers ...*/

        lay=new Vector(lays);
        cvinp=inp;
        
        for ( cl=0; cl<lays; cl++ )
        {
            if (cl==lays-1)
            {
                clay=new Layer(cvinp,out);
                lay.addElement(clay);
            }
            else
            {
                clay=new Layer(cvinp,null);
                lay.addElement(clay);

                clay.out=new Vector();
                
                if (netdescr[cl][x]>0)
                {
                    tmpout=new Vector(netdescr[cl][x]);
                    for ( cx=0; cx<netdescr[cl][x]; cx++ )
                    {
                        if (netdescr[cl][y]<=0)
                            tmpout.addElement(new Weight(initout));
                        else
                        {
                            tmpout.addElement(new Vector(netdescr[cl][y]));
                            for ( cy=0; cy<netdescr[cl][y]; cy++ )
                            {
                                if (netdescr[cl][z]<=0)
                                    ((Vector)tmpout.elementAt(cx)).addElement(new Weight(initout));
                                else
                                {
                                    ((Vector)tmpout.elementAt(cx)).addElement(new Vector(netdescr[cl][z]));
                                    for ( cz=0; cz<netdescr[cl][z]; cz++ )
                                        ((Vector)((Vector)tmpout.elementAt(cx)).elementAt(cy)).addElement(new Weight(initout));
                                }
                            }
                        }
                    }

                    clay.out.addElement(tmpout);
                }
            }

            clay.createNeurons(thrsfunc,initwgt,initlim);
            cvinp=clay.out;
        }

        return(true);
    }

    public synchronized void mirrorLayers( Network other )
    {
        lay=other.lay;
    }


    public synchronized void fire()
    {
        Object  cur;
        int     pos;
        Enumeration e;


        e=lay.elements();
        while (e.hasMoreElements())
        {
            cur=e.nextElement();
            pos=lay.indexOf(cur);
            // System.out.println("Firing layer: ("+pos+")");
            ((Layer)cur).fire();
        }
    }

    public synchronized void multi_fire( int fwinsize )
    {
        Object  cur;
        int     pos;
        Enumeration e;


        e=lay.elements();
        while (e.hasMoreElements())
        {
            cur=e.nextElement();
            pos=lay.indexOf(cur);
            // System.out.println("Firing layer: ("+pos+")");
            ((Layer)cur).group_fire(fwinsize);
        }
    }

    public synchronized void multi_prio_fire( boolean dotransf, int fwinsize, int prio )
    {
        Object  cur;
        int     pos;
        Enumeration e;


        e=lay.elements();
        while (e.hasMoreElements())
        {
            cur=e.nextElement();
            pos=lay.indexOf(cur);
            // System.out.println("Firing layer: ("+pos+")");
            ((Layer)cur).group_prio_fire(fwinsize,prio);
        }
    }


/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        int     dnet[][]=new int[2][3];

        dnet[0][0]=3;  dnet[0][1]=0;  dnet[0][2]=0;
        dnet[1][0]=1;  dnet[1][1]=0;  dnet[1][2]=0;

        Network anet=new Network();
        anet.createLayers(dnet,null,0.25,0.5,1.0,0.0);

        ((Layer)anet.lay.firstElement()).addInputFeed((Vector)anet.out.firstElement(),0.75);

        anet.multi_fire(3);
        
        System.out.println(anet.toString());
    }

}


