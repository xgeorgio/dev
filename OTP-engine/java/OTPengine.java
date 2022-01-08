/**
 * Title:        One-Time Pad Generator
 * Description:  Simple class for OTP generation. The program creates raw random data, cryptographycally secure as to be used for OTP implementations.
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author Harris Georgiou
 * @version 1.0
 */

import java.lang.*;
import java.math.*;
import java.security.*;
import java.io.*;

public class OTPengine
{
    protected static int buffSize=10000;
    private boolean isStandAlone;
    protected SecureRandom srand;

    public OTPengine()
    {
        isStandAlone=true;
        initialize();
    }

    public OTPengine( SecureRandom other )
    {
        isStandAlone=true;
        initialize(other);
    }

    public void initialize()
    {
        try
        {
            srand = SecureRandom.getInstance("SHA1PRNG", "SUN");
        }
        catch(Exception e)
        {
            try
            {
                srand = SecureRandom.getInstance("SHA1PRNG");
            }
            catch (Exception e1)
            {
                srand = new SecureRandom();
            }
        }
    }

    public void initialize( SecureRandom randeng )
    {
        srand=randeng;
    }

    public void nextBytes( byte[] bytes )
    {
        srand.nextBytes(bytes);
    }

    public boolean createBinOTP( String filename, int sizeInBytes )
    {
        BufferedOutputStream  fh;
        int         count, loops, rem;
        byte[]      buffer;

        if ((filename==null)||(sizeInBytes<=0))  return(false);

        try
            { fh=new BufferedOutputStream(new FileOutputStream(filename)); }
        catch ( Exception e )
            { return(false); }

        try
        {
            buffer=new byte[(int)Math.min(sizeInBytes,buffSize)];

            loops = (int)Math.floor(sizeInBytes / buffer.length);
            rem = sizeInBytes % buffer.length;
            System.out.println("File opened for "+sizeInBytes+" bytes  [buffer="+buffer.length+"x"+loops+"+"+rem+"]");
            
            
            System.out.print("\ninitializing PRNG byte stream...");
            System.out.flush();
            nextBytes(buffer);
            System.out.println("done!\n");

            System.out.print("generating PRNG data...");
            System.out.flush();
            
            for ( count=0; count<loops; count++ )
            {
                nextBytes(buffer);
                fh.write(buffer,0,buffer.length);
            }

            if (rem>0)
            {
                nextBytes(buffer);
                fh.write(buffer,0,rem);
            }

            System.out.println("done!\n");
            fh.close();
            return(true);
        }
        catch (Exception e)
        {
            try { fh.close(); }
            catch( Exception e0 )  {}

            return(false);
        }
    }

    public boolean createTextOTP( String filename, int sizeInBytes, boolean dotM )
    {
        BufferedWriter  fh;
        int         count, loops, rem, c1;
        byte[]      buffer;
        String      str, sep;

        if ((filename==null)||(sizeInBytes<=0))  return(false);

        try
        {
            if (dotM)  fh=new BufferedWriter(new FileWriter(filename+".m"));
            else  fh=new BufferedWriter(new FileWriter(filename));
        }
        catch ( Exception e )
            { return(false); }

        try
        {
            if (dotM)  sep=new String(",");
            else  sep=new String(" ");
            
            buffer=new byte[(int)Math.min(sizeInBytes,buffSize)];

            loops = (int)Math.floor(sizeInBytes / buffer.length);
            rem = sizeInBytes % buffer.length;
            System.out.println("File opened for "+sizeInBytes+" bytes  [buffer="+buffer.length+"x"+loops+"+"+rem+"]");
            
            
            System.out.print("initializing PRNG byte stream...");
            System.out.flush();
            nextBytes(buffer);
            System.out.println("done!");

            System.out.print("generating PRNG data...");
            System.out.flush();
            
            if (dotM)  { str=new String("prng=["); fh.write(str,0,str.length()); }
            
            for ( count=0; count<loops; count++ )
            {
                nextBytes(buffer);
                for ( c1=0; c1<buffer.length; c1++ )
                {
                    str=(new Byte(buffer[c1])).toString()+sep;
                    fh.write(str,0,str.length());
                }
            }

            if (rem>0)
            {
                nextBytes(buffer);
                for ( c1=0; c1<rem; c1++ )
                {
                    str=(new Byte(buffer[c1])).toString()+sep;
                    fh.write(str,0,str.length());
                }
            }

            if (dotM)  { str=new String("];"); fh.write(str,0,str.length()); }

            System.out.println("done!\n");
            fh.close();
            return(true);
        }
        catch (Exception e)
        {
            try { fh.close(); }
            catch( Exception e0 )  {}

            return(false);
        }
    }

    //..... main routine for standalone program .....
    public static void main(String[] args)
    {
        OTPengine OTPengine1 = new OTPengine();
        OTPengine1.isStandAlone=true;
        int         fsize=0, mode=1;
        boolean     err=false, res=false;

        System.out.println("OTP-Generator, version 1.1 - Harris Georgiou (c) 2001\n");
        
        if (args.length<2)  err=true;
        
        try { fsize=Integer.parseInt(args[1]); }
        catch ( Exception e )  { err=true; }

        if (args.length==3)
        {
            if (args[2].equals("b"))  mode=1;
            else if (args[2].equals("t"))  mode=2;
            else if (args[2].equals("m"))  mode=3;
            else  err=true;
        }

        if (args.length>3)  err=true;

        if (err)
        {
            System.out.println("Usage:  java OTPengine <output filename> <size in bytes> [b|t|m]\n");
            System.out.println("                b = binary output mode");
            System.out.println("                t = text output mode");
            System.out.println("                m = Matlab output mode\n");
            System.exit(-1);
        }

        switch (mode)
        {
            case 1: System.out.println("Output mode: BINARY");
                    res=OTPengine1.createBinOTP(args[0],fsize); break;
            case 2: System.out.println("Output mode: TEXT");
                    res=OTPengine1.createTextOTP(args[0],fsize,false); break;
            case 3: System.out.println("Output mode: MATLAB");
                    res=OTPengine1.createTextOTP(args[0],fsize,true); break;
            default:  res=false;
        }

        if (res)
        {
            System.out.println("All finished.");
            System.exit(0);
        }
        else
        {
            System.out.println("Error: unable to write to output file.");
            System.exit(-2);
        }
    }

}

