import java.lang.*;
import java.util.*;



public class CliRegistry extends Object
{
    public String   hostName, sequence, email, userid, proc;
    public String   winsize, lorejlim, hirejlim;
    public Integer  hostPort;



    public CliRegistry()
    {
        super();
    }


    public CliRegistry( String reg )
    {
        super();
        setRegistry(reg);
    }


    public boolean setRegistry( String reg )
    {
        StringTokenizer tok;

        
        hostName=new String("");
        hostPort=new Integer(0);
        sequence=new String("");
        email=new String("");
        userid=new String("");
        proc=new String("");
        winsize=new String("");
        lorejlim=new String("");
        hirejlim=new String("");
        
        tok=new StringTokenizer(reg,":");
        try
        {
            hostName=tok.nextToken();
            try  { hostPort=new Integer(Integer.parseInt(tok.nextToken())); }
            catch (NumberFormatException e0) { return(false); }
            sequence=tok.nextToken();
            email=tok.nextToken();
            userid=tok.nextToken();
            proc=tok.nextToken();
            winsize=tok.nextToken();
            lorejlim=tok.nextToken();
            hirejlim=tok.nextToken();
        }
        catch (Exception e)     // error in tokens
          { return(false); }

        return(true);
    }


    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(hostName+":");
        sbuff.append(hostPort.toString()+":");
        sbuff.append(sequence+":");
        sbuff.append(email+":");
        sbuff.append(userid+":");
        sbuff.append(proc+":");
        sbuff.append(winsize+":");
        sbuff.append(lorejlim+":");
        sbuff.append(hirejlim);

        return(sbuff.toString());
    }
        

    public boolean equals( CliRegistry clireg )
    {
        return(toString().equals(clireg.toString()));
    }


    public int hashCode()
    {
        return(toString().hashCode());
    }


/***********
    public static void main( String args[] )
    {
        CliRegistry a=new CliRegistry("aurora.eexi.gr:8881:TTTAAA:xgeorgio:555:Interactive:20:10:5");
        CliRegistry b=new CliRegistry("TAGG:667:xgeorgio");

        System.out.println(a.toString());
        System.out.println(b.toString());
        System.out.println("a==a : "+a.equals(a));
        System.out.println("a==b : "+a.equals(b));
        System.out.println("a.hashCode()="+a.hashCode());
    }
***********/

}


