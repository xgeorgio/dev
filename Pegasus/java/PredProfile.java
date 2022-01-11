import java.lang.*;
import java.util.*;
import java.io.*;



public class PredProfile extends DataProfile
{
    public PredProfile()
    {
        super();
    }


    public boolean writeProfile( String profileName )
    {
        PrintWriter     profFile;
        Enumeration     enump, enums;
        String          curKey, curField, curValue;
        Hashtable       curSeg;


        try
        {
            profFile=new PrintWriter(new FileWriter(profileName),true);
            if (profFile.checkError())
            {
                System.out.println("Error: unable to open profile ("+profileName+")");
                return(false);
            }

          /*** write two lines of comments ***/
            Date today=new Date();
            profFile.println("#"+profileTag);
            profFile.println("#"+today.toString());

            enump=prof.keys();
            while (enump.hasMoreElements())
            {
                curKey=(String)enump.nextElement();
                curSeg=(Hashtable)prof.get(curKey);

                profFile.println("["+curKey+"]");
    
                enums=curSeg.keys();
                while (enums.hasMoreElements())
                {
                    curField=(String)enums.nextElement();
                    curValue=(String)curSeg.get(curField);

                    profFile.println(curField+"="+curValue);
                }

                profFile.println();
            }
        
            profFile.close();

            if (profFile.checkError())
            {
                System.out.println("Error: unable to write to profile ("+profileName+")");
                return(false);
            }
            else
                return(true);
        }
        catch (IOException e)
        {
            System.out.println("Error: unable to open profile ("+profileName+")");
            return(false);
        }
    }


    public boolean readProfile( String profileName )
    {
        BufferedReader      profFile;
        String              str0, strL, strR;
        Hashtable           curSeg=header;
        boolean             cont=true;
        int                 eqpos=0;



        reset();

        try
        {
            profFile=new BufferedReader(new FileReader(profileName),10000);

          /*** read two lines of comments ***/
            profFile.readLine();
            profFile.readLine();
                    
            while (cont && profFile.ready())
            {
                try
                {
                    str0=profFile.readLine();
                    
                    if (str0.equals(""))
                        continue;
                    else if ( (str0.startsWith("["))&&(str0.endsWith("]")) )
                    {
                        strL=str0.substring(1,str0.length()-1);
                        // System.out.println("read segment: "+strL);
                        if (prof.containsKey(strL))
                            curSeg=(Hashtable)prof.get(strL);
                        else
                        {
                            System.out.println("error: profile: invalid segment: "+strL);
                            continue;
                        }
                    }
                    else
                    {
                        eqpos=str0.indexOf("=");
                        if (!((eqpos>0)&&(eqpos<str0.length()-1)))
                        {
                            System.out.println("error: profile: invalid record: "+str0);
                            // continue;
                        }
                        else
                        {
                            strL=str0.substring(0,eqpos);
                            strR=str0.substring(eqpos+1,str0.length());
                            curSeg.put(strL,strR);
                        }
                    }
                }
                catch (IOException e0)
                {
                    try  { profFile.close(); }  catch (IOException e00)  {}
                    cont=false;
                }
            }

            return(true);
        }
        catch (FileNotFoundException e2)
        {
            System.out.println("Error: profile not found ("+profileName+")");
            return(false);
        }
        catch (IOException e1)
        {
            System.out.println("Error: unable to read from profile ("+profileName+")");
            return(false);
        }
    }


/****************
    public static void main( String args[] )
    {
        PredProfile p=new PredProfile(), p1;
        Date d=new Date();
        boolean res;
        

        System.out.print("READING profile... ");
        res=p.readProfile("test.prof");
        if (res)
        {
            System.out.println("ok");
//            System.out.println(p.toString());
        }
        else
            System.out.println("failed");
    
        System.out.println("\nUPDATING profile...");
        p.header.put("server","Server version information tag");
        p.header.put("date",d.toString());
        System.out.println(p.toString()+"\n");
    
        System.out.print("\nCOPYING profile... ");
        p1=new PredProfile();
        p1.mirror(p);
        System.out.println(p1.toString()+"\n");

        System.out.print("\nWRITING profile... ");
        res=p.writeProfile("test.prof");
        if (res)
            System.out.println("ok");
        else
            System.out.println("failed");
    }
*****************/

}
        
    
