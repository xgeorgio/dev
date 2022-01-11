import java.lang.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;



public class ProtPredServer
{
    public PNetwork     ppnet[];
    public int      netinp=400, netout=48, result[];
    public double   initwgt=0.0, initlim=0.0, sparam=1.0, slimit=15.0;
    public int      thrd_nets=1, thrd_fire=1, iobuffsz=230976;
    public String   wdf_name="PPServer.WDF";
    public double   maxcount;

    public Properties params;
    public ProteinChars pchars;
    public Thread   t1, t2, t3;
    public ThreadGroup  tgrp;

    public static String cfgfile_name="PPServer.cfg";
    public static String paramfile_header="Protein Prediction Server Configuration File";
    public static String versionTag="Pegasus PPNet Server, version 1.02 (R981004) - Harris Georgiou (c) 1998.";
    

/*.....   Constructors   .....*/

    public ProtPredServer()
    {
        ppnet=new PNetwork[3];

        result=new int[3];

        pchars=new ProteinChars();
        
        params=new Properties();
        
        params.put("netinp","400");
        params.put("netout","48");
        params.put("initwgt","0.0");
        params.put("initlim","0.0");
        params.put("sigmoid_param","1.0");
        params.put("sigmoid_limit","15.0");
        params.put("WDF_name","PPServer.WDF");
        params.put("thrd_nets","1");
        params.put("thrd_fire","1");
        params.put("iobuffsz","462000");

        updateParams();
    }


/*.....   Class functionality methods   .....*/


/*-- standard server functions --*/

    public void printMessage( String message )
    {
        System.out.println("PPServer: "+message);
    }
    
    public void printError( String errorType )
    {
        printMessage("Error: "+errorType);
    }
    
    public boolean readParams()
    {
        BufferedInputStream    inpfile;

        try
        {
            inpfile=new BufferedInputStream(new FileInputStream(cfgfile_name),1000);
            params.load(inpfile);
            inpfile.close();
        }
        catch (IOException e)  { printError(e.toString());  return(false); }

        return(true);
    }

    public boolean writeParams()
    {
        BufferedOutputStream    outfile;

        try
        {
            outfile=new BufferedOutputStream(new FileOutputStream(cfgfile_name),1000);
            params.save(outfile,paramfile_header);
            outfile.close();
        }
        catch (IOException e)  { printError(e.toString());  return(false); }

        return(true);
    }

    public boolean updateParams()
    {
        int     ival;

        
        try
        {
            ival=Integer.parseInt(params.getProperty("netinp"));
            if (ival>0)  netinp=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        try
        {
            ival=Integer.parseInt(params.getProperty("netout"));
            if (ival>0)  netout=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        try  { initwgt=(new Double(params.getProperty("initwgt"))).doubleValue(); }
        catch (NumberFormatException e)  { return(false); }

        try  { initlim=(new Double(params.getProperty("initlim"))).doubleValue(); }
        catch (NumberFormatException e)  { return(false); }

        try  { sparam=(new Double(params.getProperty("sigmoid_param"))).doubleValue(); }
        catch (NumberFormatException e)  { return(false); }

        try  { slimit=(new Double(params.getProperty("sigmoid_limit"))).doubleValue(); }
        catch (NumberFormatException e)  { return(false); }

        try
        {
            ival=Integer.parseInt(params.getProperty("thrd_nets"));
            if (ival>0)  thrd_nets=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        try
        {
            thrd_fire=Integer.parseInt(params.getProperty("thrd_fire"));
            if (ival>0)  thrd_fire=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        try
        {
            ival=Integer.parseInt(params.getProperty("iobuffsz"));
            if (ival>0)  iobuffsz=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        wdf_name=params.getProperty("WDF_name");

        return(true);
    }


    public boolean initialize()
    {
        printMessage("initializing server...");
        if (!readParams())  return(false);
        
        printMessage("initializing networks...");

        ppnet[0]=new PNetwork(netinp,netout,new ExtSigmoidFunc(sparam,slimit),initwgt,initlim,thrd_fire);
        ppnet[1]=new PNetwork(ppnet[0].inp,netout,new ExtSigmoidFunc(sparam,slimit),initwgt,initlim,thrd_fire);
        ppnet[2]=new PNetwork(ppnet[1].inp,netout,new ExtSigmoidFunc(sparam,slimit),initwgt,initlim,thrd_fire);

        resetNetworks();

        printMessage("reading network weights...");

        return(readWeights());
    }

    public String getInfo()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(versionTag+"\n");
        sbuff.append("    "+netinp+"x"+netout+" layer");
        sbuff.append(", sigmoid(a="+sparam+",l="+slimit+")\n");

        if (thrd_nets>1)  sbuff.append("    running with concurrent ("+thrd_nets+") networks");
        else  sbuff.append("    running with iterative networks");
        
        sbuff.append(" and using "+thrd_fire+" thread(s) per network\n");

        return(sbuff.toString());
    }


/*-- analysis network functions --*/

    public void resetNetworks()
    {
        int     i;
        

        for ( i=0; i<ppnet[0].inp.length; i++ )  ppnet[0].inp[i]=0.0;       

        for ( i=0; i<ppnet[0].out.length; i++ )
            {  ppnet[0].out[i]=0.0; ppnet[1].out[i]=0.0; ppnet[2].out[i]=0.0;  }

        maxcount=0.0;

        result[0]=result[1]=result[2]=0;
    }
        

    public boolean readWeights( String name )
    {
        BufferedReader      wgtfile;
        StringTokenizer     stok;
        String      stemp, sval;
        int         netnum, npos, wpos;
        double      dval;

        
        try
        {
            wgtfile=new BufferedReader(new FileReader(name));

            netnum=0;  npos=0;  wpos=0;
            
            printMessage("reconstructing network "+(netnum+1));

            while (wgtfile.ready())
            {
                stemp=wgtfile.readLine();

                stok=new StringTokenizer(stemp,",");
                if (!stok.hasMoreTokens())  break;
                stok.nextToken();
                stok.nextToken();
                sval=stok.nextToken();

                try  { dval=(new Double(sval)).doubleValue(); }
                catch (NumberFormatException e)
                {
                    printError("invalid weight value in WDF file ("+wdf_name+")");
                    return(false);
                }

                if (wpos<netinp)
                {
                    ppnet[netnum].neu[npos].wgt[wpos]=dval;
                    wpos++;
                }
                else
                {
                    ppnet[netnum].neu[npos].lim=dval;
                    npos++;  wpos=0;

                    if (npos>=netout)
                    {
                        netnum++;  npos=0;
                        if (netnum<=2)  printMessage("reconstructing network "+(netnum+1));
                    }
                }
            }

            wgtfile.close();
        }
        catch (IOException e)
            {  printError(e.toString());  return(false);  }

        return(true);
    }


    public boolean readWeights()
    {
        DataInputStream    wgtfile;
        int         netnum, npos, wpos;
        float       fval;


        try
        {
            //--- uncompressed .WDF version: ---> wgtfile=new DataInputStream(new BufferedInputStream(new FileInputStream(wdf_name),iobuffsz));
            wgtfile=new DataInputStream(new BufferedInputStream(new InflaterInputStream(new FileInputStream(wdf_name)),iobuffsz));

            for ( netnum=0; netnum<ppnet.length; netnum++ )
            {
                for ( npos=0; npos<ppnet[netnum].neu.length; npos++ )
                {
                    for ( wpos=0; wpos<ppnet[netnum].neu[npos].wgt.length; wpos++ )
                    {
                        fval=wgtfile.readFloat();
                        ppnet[netnum].neu[npos].wgt[wpos]=(double)fval;
                    }

                    fval=wgtfile.readFloat();
                    ppnet[netnum].neu[npos].lim=(double)fval;
                }
            }

            wgtfile.close();
        }
        catch (IOException e)
            {  printError(e.toString()); return(false);  }

        return(true);
    }

    public boolean writeWeights()
    {
        DataOutputStream    wgtfile;
        int         netnum, npos, wpos;
        float       fval;


        try
        {
            //--- uncompressed .WDF version: ---> wgtfile=new DataOutputStream(new BufferedOutputStream(new FileOutputStream(wdf_name),iobuffsz));
            wgtfile=new DataOutputStream(new BufferedOutputStream(new DeflaterOutputStream(new FileOutputStream(wdf_name)),iobuffsz));

            for ( netnum=0; netnum<ppnet.length; netnum++ )
            {
                for ( npos=0; npos<ppnet[netnum].neu.length; npos++ )
                {
                    for ( wpos=0; wpos<ppnet[netnum].neu[npos].wgt.length; wpos++ )
                    {
                        fval=(float)ppnet[netnum].neu[npos].wgt[wpos];
                        wgtfile.writeFloat(fval);
                    }

                    fval=(float)ppnet[netnum].neu[npos].lim;
                    wgtfile.writeFloat(fval);
                }
            }

            wgtfile.close();
        }
        catch (IOException e)
            {  printError(e.toString()); return(false);  }

        return(true);
    }

        
    public boolean translateInput( String inpstr )
    {
        Character    pch0, pch;
        int     xpos, ypos, tmp, i;


        if (!(pchars.isValid(inpstr)))
            {  printError("Invalid input sequence");  return(false);  }
        
        pch=new Character(inpstr.charAt(0));
        for ( i=1; i<inpstr.length(); i++ )
        {
            pch0=new Character(pch.charValue());
            pch=new Character(inpstr.charAt(i));

            xpos=pchars.indexOf(pch);
            ypos=pchars.indexOf(pch0);
            if (ypos>xpos)
                {  tmp=xpos; xpos=ypos; ypos=tmp;  }

            tmp=xpos*20+ypos;
            
            ppnet[0].inp[tmp]++;
            if (ppnet[0].inp[tmp]>maxcount)  maxcount=ppnet[0].inp[tmp];
        }

        //--- Note: set 'maxout' to 16.0 if maximum is not to be used ---
        // maxcount=16.0;
        
        for ( i=0; i<ppnet[0].inp.length; i++ )
            if (ppnet[0].inp[i]>0.0)  ppnet[0].inp[i]=ppnet[0].inp[i]/maxcount;

        return(true);
    }

    public int transNetworkOutput( int netnum )
    {
        int     i, maxpos;


        if ((netnum<0)||(netnum>2))
            {  printError("Invalid network number");  return(-2);  }

        for ( i=0,maxpos=0; i<ppnet[netnum].out.length; i++ )
            if ((ppnet[netnum].out[i]>0.0)&&(ppnet[netnum].out[i]>ppnet[netnum].out[maxpos]))
                maxpos=i;

        return(maxpos);
    }
        
    public boolean translateOutput()
    {
        result[0]=transNetworkOutput(0);
        result[1]=transNetworkOutput(1);
        result[2]=transNetworkOutput(2);

        return((result[0]>=0)&&(result[1]>=0)&&(result[2]>=0));
    }


    public synchronized boolean calculate( String inpstr )
    {
        printMessage("firing networks");

        tgrp=new ThreadGroup("PPServer group");
        
        resetNetworks();

        if (translateInput(inpstr)==false)  return(false);
        
        if (thrd_nets<3)
        {
            if (thrd_fire<=1)
            {
                printMessage("\tfiring network 1 (iterative,single-threaded)");
                ppnet[0].fire();
                
                printMessage("\tfiring network 2 (iterative,single-threaded)");
                ppnet[1].fire();

                printMessage("\tfiring network 3 (iterative,single-threaded)");
                ppnet[2].fire();
            }
            else  /* fire multiple neurons */
            {
                printMessage("\tfiring network 1 (iterative,threads="+thrd_fire+")");
                ppnet[0].multi_fire(thrd_fire);
                
                printMessage("\tfiring network 2 (iterative,threads="+thrd_fire+")");
                ppnet[1].multi_fire(thrd_fire);

                printMessage("\tfiring network 3 (iterative,threads="+thrd_fire+")");
                ppnet[2].multi_fire(thrd_fire);
            }
        }
        else  /* fire networks concurrently, with 1 or more threads each */
        {
            t1=new Thread(tgrp,ppnet[0]);
            t2=new Thread(tgrp,ppnet[1]);
            t3=new Thread(tgrp,ppnet[2]);

            printMessage("\tfiring network 1 (concurrent,threads="+thrd_fire+")");
            t1.start();

            printMessage("\tfiring network 2 (concurrent,threads="+thrd_fire+")");
            t2.start();

            printMessage("\tfiring network 3 (concurrent,threads="+thrd_fire+")");
            t3.start();

            while (tgrp.activeCount()>0)  Thread.currentThread().yield();
            tgrp.stop();
        }
        
        return(translateOutput());
    }


/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        ProtPredServer ppserver=new ProtPredServer();
        boolean     res=false;

        System.out.println(ppserver.getInfo());

        if (args.length==0)     //---> "setup" mode
        {
            ppserver.printMessage("*** SETUP MODE ***");
            ppserver.printMessage("initializing server...");
            ppserver.initialize();

            res=ppserver.readWeights("weights.txt");

            if (res)
            {
                ppserver.printMessage("saving all network weights...");
                res=res && ppserver.writeWeights();
            }

            if (res)  ppserver.printMessage("finished");
            else  ppserver.printError("unable to complete setup");
        }
        else    //---> "prediction" mode
        {
            args[0]=args[0].toUpperCase();

            ppserver.printMessage("initializing server...");
            ppserver.initialize();

            ppserver.printMessage("executing prediction...");
            res=ppserver.calculate(args[0]);

            if (res)  ppserver.printMessage("finished:\ninput sequence="+args[0]+"\nresult category={"
                                    +ppserver.result[0]+","+ppserver.result[1]+","+ppserver.result[2]+"}");
            else  ppserver.printError("unable to execute prediction");
        }
    }

}



