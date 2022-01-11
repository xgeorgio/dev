import java.lang.*;
import java.io.*;
import java.net.*;
import java.util.*;



public class MailServer extends Object
{
    public boolean  internal;
    public String   host;
    public int      port;
    public String   extmailcmd;
    public String   returnaddr;

    public Socket   sessionSock;
    public BufferedReader    inStream;
    public DataOutputStream  outStream;

    public Properties params;

    public static String mailfile_name="tosend.txt";
    public static int mailfile_buff=10000;

    public static String cfgfile_name="MailServer.cfg";
    public static String paramfile_header="Mail Server Configuration File";
    public static String versionTag="Pegasus Mail Server, version 1.01 (R981004) - Harris Georgiou (c) 1998.";

    

/*.....   Constructor(s)   .....*/

    public MailServer()
    {
        params=new Properties();

        params.put("internal","true");
        params.put("host","127.0.0.0");
        params.put("port","25");
        params.put("extmailcmd","rmail_dos.bat");
        params.put("returnaddr","root@127.0.0.0");

        updateParams();
        
        if (initialize())  updateParams();
    }


/*.....   Class functionality methods   .....*/


/*-- standard server functions --*/

    public void printMessage( String message )
    {
        System.out.println("MailServer: "+message);
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


        internal=Boolean.getBoolean(params.getProperty("internal"));

        host=params.getProperty("host");
        
        try
        {
            ival=Integer.parseInt(params.getProperty("port"));
            if (ival>0)  port=ival;  else  return(false);
        }           
        catch (NumberFormatException e)  { return(false); }

        extmailcmd=params.getProperty("extmailcmd");

        returnaddr=params.getProperty("returnaddr");

        return(true);
    }

    public String getInfo()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(versionTag+"\n");
        if (internal)
        {
            sbuff.append("   using internal SMTP mailer");
            sbuff.append(" (SMTPserv='"+host+":"+port+"',");
            sbuff.append(" retrnaddr='"+returnaddr+"')\n");
        }
        else
        {
            sbuff.append("   using external mail spooler");
            sbuff.append(" (script='"+extmailcmd+"',");
            sbuff.append(" retrnaddr='"+System.getProperty("user.name","(unknown)")+"')\n");
        }

        return(sbuff.toString());
    }

    public boolean initialize()
    {
        printMessage("initializing server...");
        return(readParams());
    }

        
/*-- SMTP sender functions --*/

    public void openConnection() throws IOException
    {
        sessionSock=new Socket(host,port);
        inStream=new BufferedReader(new InputStreamReader(sessionSock.getInputStream()),1000);
        outStream=new DataOutputStream(sessionSock.getOutputStream());
    }

    public void closeConnection()
    {
        try
        {
            if (sessionSock!=null)  sessionSock.close();
            if (inStream!=null)   inStream.close();
            if (outStream!=null)  outStream.close();
        }
        catch (IOException e)  {}
        finally
            {  sessionSock=null; inStream=null; outStream=null;  }
    }

    public String doCommand( String cmdstr ) throws IOException
    {
        outStream.writeBytes(cmdstr+"\n");
        String response=getResponse();

        return(response);
    }

    public String getResponse() throws IOException
    {
        String  response="", line;

        for (;;)
        {
            line=inStream.readLine();

            if (line==null)
                throw(new IOException("Bad response from server."));
            if (line.length()<3)
                throw(new IOException("Bad response from server."));

            response=response+line+"\n";

            if ((line.length()==3)||(line.charAt(3)!='-'))
                return(response);
        }
    }

    public void sendMessage( String recipient, String message[] ) throws IOException
    {
        String  response;


        printMessage("initializing SMTP connection ("+host+":"+port+")");
        
        openConnection();

        response=getResponse();
        if (response.charAt(0)!='2')  throw(new IOException(response));

        response=doCommand("HELO");
        if (response.charAt(0)!='2')  throw(new IOException(response));

        response=doCommand("MAIL FROM: "+returnaddr);
        if (response.charAt(0)!='2')  throw(new IOException(response));

        response=doCommand("RCPT TO: "+recipient);
        if (response.charAt(0)!='2')  throw(new IOException(response));
    
        response=doCommand("DATA");
        if (response.charAt(0)!='3')  throw(new IOException(response));

        printMessage("sending SMTP data to recipient ("+recipient+")");

        for ( int i=0; i<message.length; i++ )
        {
            if (message[i].length()==0)
                {  outStream.writeBytes("\n"); continue;  }

            if (message[i].charAt(0)=='.')
                outStream.writeBytes("."+message[i]+"\n");
            else
                outStream.writeBytes(message[i]+"\n");
        }

        response=doCommand(".");
        if (response.charAt(0)!='2') throw(new IOException(response));

        closeConnection();
    }


/*-- external mailer function(s) --*/

    public void executeMailer( String recipient, String message[] )
        throws IOException, InterruptedException
    {
        BufferedWriter    mailfile;


        printMessage("creating mail data file ("+mailfile_name+")");
        mailfile=new BufferedWriter(new FileWriter(mailfile_name),mailfile_buff);
        for ( int i=0; i<message.length; i++ )  mailfile.write(message[i]+"\n");
        mailfile.close();
        
        printMessage("executing external mail spooler ("+extmailcmd+" "+recipient+")");
        Runtime.getRuntime().exec(extmailcmd+" "+recipient).waitFor();
    }


/*-- global mail sender function --*/

    public boolean sendMail( String recipient, String message[] )
    {
        try
        {
            if (internal)
            {
                printMessage("sending mail via SMTP server");
                sendMessage(recipient,message);
            }
            else
            {
                printMessage("sending mail via external mailer");
                executeMailer(recipient,message);
            }

            return(true);
        }
        catch (Exception e)
        {
            printError(e.toString());
            return(false);
        }
    }
    

/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        MailServer msvr=new MailServer();
        String  msg[];


        System.out.println(msvr.getInfo());
        msvr.initialize();
        
        msg=new String[2];
        msg[0]=new String("mpla mpla MPLA");
        msg[1]=new String("more mpla mpla");

        msvr.internal=false;
        msvr.sendMail("xgeorgio@compulink.gr",msg);

        msvr.internal=true;
        msvr.sendMail("xgeorgio@compulink.gr",msg);        
    }

}


