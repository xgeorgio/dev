import java.io.*;
import java.net.*;
import java.lang.*;
import java.util.*;


public class RemoteMan
{
    BufferedReader     inStream;
    PrintWriter        outStream;
    Socket             s;

     
    public RemoteMan( String host, int port, String command )
    {
        String          cmdline;
        StringBuffer    sbuff;

        
    	try
    	{
    	    s=new Socket(host,port);
	        inStream=new BufferedReader(new InputStreamReader(s.getInputStream()),1000);
	        outStream=new PrintWriter(s.getOutputStream(),true);
	    
    	    doCommand(command);
    	}
	    catch (IOException e1)
    	{
	        System.out.println("Error: I/O error occurred.");
	        System.out.println(e1.toString());
    	}
  	    finally
      	{
   	        try
   	          { inStream.close(); outStream.close(); s.close(); }
       	    catch (Exception e4)  {}
   	    }
    }

    public void doCommand( String cmd )
    {
        String    resp;
        StringBuffer sbuff=new StringBuffer();
        
        
        for (int i=0; i<cmd.length(); i++ )
            outStream.print(cmd.charAt(i));
            
        if (!outStream.checkError())
        {
            System.out.println("REMOTE COMMAND:\n"+cmd);

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
        String  cmdline;
        StringBuffer    sbuff=new StringBuffer();
        BufferedReader  userin=new BufferedReader(new InputStreamReader(System.in),100);

        
        System.out.println("Generic Remote Manager, version 2.0 (R981004) - Harris Georgiou (c) 1998.");
        try
        {
            String host=argv[0];
            int port=Integer.parseInt(argv[1]);
            String command;

            if (argv.length>=3)
            {
                for ( int i=2; i<argv.length; i++ )
                  { sbuff.append(argv[i]);  sbuff.append("\n"); }
	                
                command=sbuff.toString();
            }
            else
            {
	            System.out.println("Type the command(s) ('.' for end):");
	            try
	            {
                    cmdline=userin.readLine();
	                while(!cmdline.equals("."))
	                {
                        sbuff.append(cmdline);
                        sbuff.append("\n");
	                    cmdline=userin.readLine();
	                }
	                
	                command=sbuff.toString();
	            }
	            catch (IOException e)
	            {
	                System.out.println("Error: unable to read from <stdin>");
	                command=null;
	            }
            }

            if (command==null)  System.exit(-1);
            
            RemoteMan sm=new RemoteMan(host,port,command);
        }
     	catch (ArrayIndexOutOfBoundsException e2)
        { 
            System.out.println("Usage: java RemoteMan <host> <port> [<command>]");
   	    }
    	catch (NumberFormatException e3)
    	{
    	    System.out.println("Error: invalid port number");
    	}
    }
     
}


