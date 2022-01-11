import java.lang.*;
import java.util.*;
import java.io.*;
import java.net.*;



public class PRManager
{
    public String   host;
    public int      port;
    public boolean  useTTY;
    public Properties   params;

    public String   cmdstr[];
    public int      cmdnum;
    
    public static String cfgfile_name="PRManager.cfg";
    public static String paramfile_header="Pegasus Remote Manager Configuration File";
    public static String versionTag="Pegasus Remote Manager, version 2.0 (R981004) - Harris Georgiou (c) 1998.";
    

/*.....   Constructors   .....*/

    public PRManager()
    {
        cmdstr=new String[4];
        cmdstr[0]=new String("end of command list\n");
        cmdstr[1]=new String(Messages.commandStart+":"+Messages.rcmdAck);
        cmdstr[2]=new String(Messages.commandStart+":"+Messages.rcmdInfo);
        cmdstr[3]=new String(Messages.commandStart+":"+Messages.rcmdQuit);
        
        params=new Properties();
        
        params.put("host","127.0.0.0");
        params.put("port","8881");
        params.put("useTTY","true");

        updateParams();
    }


/*.....   Class functionality methods   .....*/

    public void printMessage( String message )
    {
        System.out.println("PRManager: "+message);
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

        
        try  { ival=Integer.parseInt(params.getProperty("port"));
               if (ival>0)  port=ival;  else  return(false); }           
        catch (NumberFormatException e)  { return(false); }

        useTTY=Boolean.valueOf(params.getProperty("useTTY")).booleanValue();
        
        host=params.getProperty("host");

        return(true);
    }


    public boolean initialize()
    {
        // printMessage("initializing...");

        cmdnum=0;
        if (!readParams())  return(false);

        return(updateParams());
    }

    public String getInfo()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append(versionTag+"\n");
        sbuff.append("configured for Pegasus server at '"+host+":"+port+"'\n");

        sbuff.append("TTY (console) interface is ");
        if (useTTY) sbuff.append("ON");  else sbuff.append("OFF");

        sbuff.append("\n");

        return(sbuff.toString());
    }
    

    public boolean doCommand( String cmd )
    {
        Socket          sock;
        String          resp;
        BufferedReader  inpStream;
        PrintWriter     outStream;
        StringBuffer    sbuff=new StringBuffer();
        boolean         res;
        

        try
        {
    	    sock=new Socket(host,port);
	        inpStream=new BufferedReader(new InputStreamReader(sock.getInputStream()),1000);
	        outStream=new PrintWriter(sock.getOutputStream(),true);
        }
	    catch (IOException e1)
    	{
	        printError(e1.toString());
	        return(false);
    	}

        cmdnum++;
        System.out.println("REMOTE COMMAND #"+cmdnum+":\n"+cmd);

        outStream.println(cmd);
        if (!outStream.checkError())
        {
            System.out.println("\nSERVER ANSWER #"+cmdnum+":");
            try
            {
                resp=inpStream.readLine();
                sbuff.append(resp.replace('$','\n')+"\n");
            }
            catch (IOException e0)  { sbuff.append("\n"); }

            printMessage(sbuff.toString());
            res=true;
        }
        else
        {
            printError("Unable to send command (aborted)");
            res=false;
        }

        try  { inpStream.close(); outStream.close(); sock.close(); }
	    catch (IOException e)  { printError(e.toString()); }

	    return(res);
    }
    	        
    public int execCommandTTY()
    {
        String      cmdnum, command;
        StringBuffer    sbuff;
        BufferedReader     userin=new BufferedReader(new InputStreamReader(System.in),100);
        boolean     validcmd;
        int         res=0;

        
        try
        {
            validcmd=false;
            do
            {
                System.out.println("\nPegasus - Remote Commands Menu");
                System.out.println("------------------------------\n");
                System.out.println("\t1. Acknowledge\n");
                System.out.println("\t2. Status informations\n");
                System.out.println("\t3. Shutdown server\n");
                System.out.println("\t0. EXIT\n");

                System.out.print("\nSelect command number:  ");
                System.out.flush();
                cmdnum=userin.readLine();
                System.out.println();

                if (cmdnum.equals("0"))
                {
                    System.out.println("exiting remote manager...\n");
                    validcmd=true;
                    res=0;
                }
                else if (cmdnum.equals("1"))
                {
                    System.out.println("executing remote command...");
                    command=new String(Messages.commandStart+":"+Messages.rcmdAck);
                    validcmd=true;
                    if (doCommand(command))  res=1;  else  res=-1;
                }
                else if (cmdnum.equals("2"))
                {
                    System.out.println("executing remote command...");
                    command=new String(Messages.commandStart+":"+Messages.rcmdInfo);
                    validcmd=true;
                    if (doCommand(command))  res=2;  else  res=-2;
                }
                else if (cmdnum.equals("3"))
                {
                    System.out.println("executing remote command...");
                    command=new String(Messages.commandStart+":"+Messages.rcmdQuit);
                    validcmd=true;
                    if (doCommand(command))  res=3;  else  res=-3;
                }
                else
                {
                    System.out.println("Invalid command number\n");
                    validcmd=false;
                }
    	    }
    	    while (!validcmd);
    	}
	    catch (IOException e1)
    	{
	        printError("console I/O (read) error occurred");
	        res=-100;
    	}

    	return(res);
    }

    public boolean start( int commands[] )
    {
        boolean     res=true;
        int         ires;

        
        if ((commands.length>0)&&(commands[0]>=0))
        {
            printMessage("executing remote command(s)...");

            for ( int i=0; (res)&&(i<commands.length); i++ )
            {
                if (commands[i]==0)  break;
                
                res = res && doCommand(cmdstr[commands[i]]);
            }

            return(res);
        }

        if (useTTY)
        {
            do
            {
                ires=execCommandTTY();
                if ((res)&&(ires<0))  res=false;
            }
            while (ires!=0);

            return(res);
        }

        //... no command-line args and no TTY input
        printError("you must give at least one command-line instruction");
        return(false);
    }
            

/*.....   Class testing module   .....*/

    public static void main( String args[] )
    {
        PRManager prman=new PRManager();
        int         commands[], ival, i, cmdstart=0;
        boolean     parsecmd=false;
        Vector      usageOptions=new Vector();


        usageOptions.addElement("-?");
        usageOptions.addElement("-h");
        usageOptions.addElement("-H");
        usageOptions.addElement("-help");
        usageOptions.addElement("-HELP");
        usageOptions.addElement("/?");
        usageOptions.addElement("/h");
        usageOptions.addElement("/H");
        usageOptions.addElement("/help");
        usageOptions.addElement("/HELP");

        prman.initialize();

        commands=new int[args.length];

        if (args.length>0)
        {
            commands[0]=-1;

            if (usageOptions.contains(args[0]))
            {
                System.out.println("Usage: java PRManager ['-host:'<host> '-port:'<port>] [<commandlist>]");
                System.exit(0);
            }
                
            try
            {
                for ( i=0; i<args.length; i++ )
                {
                    if (args[i].startsWith("-host:"))
                    {
                        if (parsecmd)  throw(new NumberFormatException());
                        prman.params.put("host",args[i].substring(6));
                        prman.updateParams();
                    }
                    else if (args[i].startsWith("-port:"))
                    {
                        if (parsecmd)  throw(new NumberFormatException());
                        prman.params.put("port",args[i].substring(6));
                        prman.updateParams();
                    }
                    else
                    {
                        ival=Integer.parseInt(args[i]);
                        if ((ival<0)||(ival>3))  throw(new NumberFormatException());

                        if (!parsecmd)
                            {  parsecmd=true; cmdstart=i;  }

                        commands[i-cmdstart]=ival;
                    }
                }
            }
            catch (NumberFormatException e)
            {
                System.out.println("Error: Invalid command line parameters");
                System.out.println("Usage: java PRManager ['-host:'<host> '-port:'<port>] [<commandlist>]");
                System.exit(-100);
            }
        }

        System.out.println(prman.getInfo());
        prman.start(commands);
        
        // prman.printMessage("writing configuration file...");
        // prman.writeParams();
    }

}

