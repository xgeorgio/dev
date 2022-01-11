import java.lang.*;
import java.io.*;
import java.net.*;
import java.util.*;



public class DataProfile extends Object
{
    public Hashtable    prof, header, inpseq, params, totalp;
    public Hashtable    windowp, clsfreq, clscorrl, clsnotes;
    protected static String profileTag="Pegasus_Data_Profile_V2.01";
    protected static String finishTag="<ENDPROFILE>";
    

    public DataProfile()
    {
        prof=new Hashtable(12);

        header=new Hashtable(10);
        inpseq=new Hashtable(5);
        params=new Hashtable(10);
        totalp=new Hashtable(5);
        windowp=new Hashtable();
        clsfreq=new Hashtable();
        clscorrl=new Hashtable();
        clsnotes=new Hashtable();

        prof.put("header",header);
        prof.put("inpseq",inpseq);
        prof.put("params",params);
        prof.put("totalp",totalp);
        prof.put("windowp",windowp);
        prof.put("clsfreq",clsfreq);
        prof.put("clscorrl",clscorrl);
        prof.put("clsnotes",clsnotes);

        header.put("versionTag",profileTag);
    }


    public boolean equals( Object anotherObject )
    {
        return( prof.equals(((DataProfile)anotherObject).prof) );
    }


    public int hashCode()
    {
        return(prof.hashCode());
    }

    
    public String toTransString()
    {
        Enumeration     enump, enums;
        String          curKey, curField, curValue;
        Hashtable       curSeg;
        StringBuffer    sbuff=new StringBuffer();


        enump=prof.keys();
        while (enump.hasMoreElements())
        {
            curKey=(String)enump.nextElement();
            curSeg=(Hashtable)prof.get(curKey);

            sbuff.append("["+curKey+"]&");
    
            enums=curSeg.keys();
            while (enums.hasMoreElements())
            {
                curField=(String)enums.nextElement();
                curValue=(String)curSeg.get(curField);
                sbuff.append(curField+"="+curValue+"&");
            }
        }
        
        /*sbuff.append(finishTag);*/    // send end-of-profile tag to socket
        return(sbuff.toString());
    }


    public boolean transString( String readFrom )
    {
        String          str0, strL, strR;
        Hashtable       curSeg=header;
        boolean         cont=true;
        int             eqpos=0;
        StringTokenizer stok=new StringTokenizer(readFrom,"&");


        reset();
        
        while (cont && stok.hasMoreTokens())
        {
            str0=stok.nextToken();
                
            if (str0.equals(""))  cont=false;
            else if (str0.equals(finishTag))  cont=false;
            else if ( (str0.startsWith("["))&&(str0.endsWith("]")) )
            {
                strL=str0.substring(1,str0.length()-1);
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
                if (!(eqpos>0))
                {
                    System.out.println("error: profile: invalid record: "+str0);
                    continue;
                }
                else if (!(eqpos<str0.length()-1))
                {
                    //System.out.println("warning: profile: null record: "+str0);
                    strL=str0.substring(0,eqpos);
                    strR=new String("");
                    curSeg.put(strL,strR);
                }
                else
                {
                    strL=str0.substring(0,eqpos);
                    strR=str0.substring(eqpos+1,str0.length());
                    curSeg.put(strL,strR);
                }
            }
        }

        return(true);
    }


    public boolean sendProfile( Socket profDest )
    {
        DataOutputStream    profFile;
        String              prftrans;
        

        try
        {
            //profFile=new DataOutputStream(profDest.getOutputStream());
            profFile=new DataOutputStream(new BufferedOutputStream(profDest.getOutputStream(),10000));

            prftrans=toTransString();
            //System.out.println("PROFILE SENT:\n"+prftrans);

            profFile.writeBytes(prftrans+"\n");
            profFile.flush();
            
            //profFile.close();  ---> ERROR: closes the socket too!!!
            
            return(true);
        }
        catch (Exception e)
          { System.out.println("Error: unable to open socket ("+profDest.toString()+")");
            return(false); }
    }


    public boolean recvProfile( Socket profFrom )
    {
        //DataInputStream      profFile;
        BufferedReader         profFile;
        String               prftrans;
        char   ch;
        

        try
          //{ profFile=new DataInputStream(profFrom.getInputStream()); }
          { profFile=new BufferedReader(new InputStreamReader(profFrom.getInputStream()),10000); }
        catch (Exception e1)
          { System.out.println("Error: unable to open socket ("+profFrom.toString()+")");
            return(false); }

        try
        {
            prftrans=profFile.readLine();
            //System.out.println("RECEIVED PROFILE:\n"+prftrans);

            transString(prftrans);

        }
        catch (IOException e2)
          { System.out.println("Error: unable to read from socket ("+profFrom.toString()+")");
            try { profFile.close(); }  catch (IOException e02)  {}
            return(false); }
        
        //try { profFile.close(); }  catch (IOException e3)  {}  ---> ERROR: closes the socket too!!!

        return(true);
    }


    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();
        Enumeration enum=prof.keys();
        String      k;
        Hashtable   h;
        

        sbuff.append("[  data profile:\n");
        while (enum.hasMoreElements())
        {
            k=(String)enum.nextElement();
            h=(Hashtable)prof.get(k);
            sbuff.append("\t"+k+" :  "+h.toString()+"\n");
        }
        sbuff.append("]");

        return(sbuff.toString());
    }


    public void reset()
    {
        Enumeration     enum;
        String          curKey;
        Hashtable       curSeg;


        enum=prof.keys();
        while (enum.hasMoreElements())
        {
            curKey=(String)enum.nextElement();
            curSeg=(Hashtable)prof.get(curKey);
            curSeg.clear();
        }

        header.put("versionTag",profileTag);
    }
    

    public boolean mirror( DataProfile from )
    {
      /*** Note: creates an internal reference, but 'equals' returns 'false' ***/
        Enumeration     enump, enums;
        String          curKey, curField, curValue;
        Hashtable       curSeg, frmSeg;


        try
        {
            enump=prof.keys();
            while (enump.hasMoreElements())
            {
                curKey=(String)enump.nextElement();
                curSeg=(Hashtable)prof.get(curKey);
                frmSeg=(Hashtable)from.prof.get(curKey);

                curSeg.clear();

                enums=frmSeg.keys();
                while (enums.hasMoreElements())
                {
                    curField=(String)enums.nextElement();
                    curValue=(String)frmSeg.get(curField);
                    curSeg.put(curField,curValue);
                }
            }

            return(true);
        }
        catch (Exception e)
        {
            return(false);  // unexpected error (i.e.: null pointer)
        }
    }


/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        DataProfile pf=new DataProfile();

        System.out.println(pf.toString());
    }
    
}
        
    
