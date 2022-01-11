import java.io.*;
import java.net.*;
import java.lang.*;
import java.util.*;


public class RemoteSMan
{
    BufferedReader     inStream;
    PrintWriter        outStream;
    Socket             s;
    String             svrhost;
    int                svrport;

   
    public RemoteSMan( String host, int port, int commandarg )
    {
        String          cmdnum, command;
        StringBuffer    sbuff;
        BufferedReader  userin=new BufferedReader(new InputStreamReader(System.in),100);
        boolean         validcmd=false;


        svrhost=host;
        svrport=port;
        
        try
        {
            if ((commandarg>=0)&&(commandarg<=3))
                {  validcmd=true; System.out.println("Executing external command selection: "+commandarg); }

            while (!validcmd)
            {
                System.out.println("\nRemote commands menu");
                System.out.println("--------------------\n");
                System.out.println("\t1. Acknowledge\n");
                System.out.println("\t2. Status informations\n");
                System.out.println("\t3. Shutdown server\n");
                System.out.println("\t0. EXIT\n");

                System.out.print("\nSelect command number:  ");
                System.out.flush();
                cmdnum=userin.readLine();

                try
                {
                    commandarg=Integer.parseInt(cmdnum);
                    validcmd=((commandarg>=0)&&(commandarg<=3));
                }
                catch (NumberFormatException e)
                    {  System.out.println("Invalid command number\n");  }
            }
            
            if (commandarg==0)
                {  System.out.println("Operation aborted\n");  }
            else if (commandarg==1)
                {  doCommand(new String(Messages.commandStart+":"+Messages.rcmdAck));  }
            else if (commandarg==2)
                {  doCommand(new String(Messages.commandStart+":"+Messages.rcmdInfo));  }
            else // if (commandarg==3)
                {  doCommand(new String(Messages.commandStart+":"+Messages.rcmdQuit));  }
    	}
	    catch (IOException e1)
    	{
	        System.out.println("Error: I/O error occurred.");
	        System.out.println(e1.toString());
    	}
    }

    public void doCommand( String cmd )
    {
        String    resp;
        StringBuffer sbuff=new StringBuffer();

        try
        {
    	    s=new Socket(svrhost,svrport);
	        inStream=new BufferedReader(new InputStreamReader(s.getInputStream()),1000);
	        outStream=new PrintWriter(s.getOutputStream(),true);
        }
	    catch (IOException e1)
    	{
	        System.out.println("Error: I/O error occurred.");
	        System.out.println(e1.toString());
	        System.exit(-1);
    	}

        System.out.println("REMOTE COMMAND:  "+cmd);

        outStream.println(cmd);
        if (!outStream.checkError())
        {
            System.out.println("\nSERVER ANSWER:");
            try
            {
                resp=inStream.readLine();
                sbuff.append(resp.replace('$','\n')+"\n");
            }
            catch (IOException e0)  { sbuff.append("\n"); }

            System.out.println(sbuff.toString());
        }
        else
            System.out.println("Error: Unable to send command");
    }
    	        

    public static void main( String argv[] )
    {
        System.out.println("Pegasus Remote 'Simple' Manager, version 2.0 (R981004) - Harris Georgiou (c) 1998.");

        try
        {
            String host=argv[0];
            int port=Integer.parseInt(argv[1]);
            int commandarg=-1;

            if (argv.length>=3)  commandarg=Integer.parseInt(argv[2]);

            RemoteSMan sm=new RemoteSMan(host,port,commandarg);
        }
     	catch (ArrayIndexOutOfBoundsException e2)
        { 
            System.out.println("Usage: java RemoteSMan <host> <port> [<commandnum>]");
   	    }
    	catch (NumberFormatException e3)
    	{
            System.out.println("Usage: java RemoteSMan <host> <port> [<commandnum>]");
    	}
    }
     
}


