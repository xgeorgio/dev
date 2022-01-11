import java.awt.*;
import java.util.*;
import java.io.*;
import java.net.*;



public class SvrMain // extends Frame
{
    protected int   cAcc, cRun, cHold;
    protected Date  currDate;
    protected long  memFree, memTotal;
    protected boolean   serverRunning;
    
    protected Socket        cliSock;
    protected ServerSocket  svrSock;
    protected String        svrHost;
    protected int           svrPort;
    protected PrintWriter   logFile;
    protected int       cachesize;
    protected boolean       tty, log, usecache, useDB, usequeue;
    protected SvrConfig     cfg;
    protected CliRegistry   reg;
    protected CliProcess    cli;
    
    public ProfileCache     cache;
    public UserDatabase     users;
    public ClientQueue      queue;
    public ProtPredServer   predictor;
    public MailServer       mailer;
    
    public final static String cfgHeader="Pegasus Server Configuration File";
    public final static String versionTag="PEGASUS Server, version 2.0 (R981004) - Harris Georgiou (c) 1998.";
    public final static String windowTag="Pegasus Server";
    public final static String cfgFileName="SvrMain.cfg";    
    public final static String mailProgram="rmail -s Pegasus_server__Prediction_results";    



    public static void main( String args[] )
    {
        SvrMain    app=new SvrMain();
    }
   
    public SvrMain()
    {
        String  logFileName;
        
      /*** Read parameters from configuration file ***/
        cfg=new SvrConfig(cfgFileName,cfgHeader);
        cfg.readParamFile();

        try
          { svrPort=Integer.valueOf(cfg.params.getProperty("port")).intValue(); }
        catch (NumberFormatException e)
          { System.out.println("Error: invalid configuration file");
            System.exit(-1); }

        try
          { cachesize=Integer.valueOf(cfg.params.getProperty("cachesize")).intValue(); }
        catch (NumberFormatException e)
          { System.out.println("Error: invalid configuration file");
            System.exit(-1); }

        tty=Boolean.valueOf(cfg.params.getProperty("tty")).booleanValue();
        log=Boolean.valueOf(cfg.params.getProperty("log")).booleanValue();
        usecache=Boolean.valueOf(cfg.params.getProperty("cache")).booleanValue();
        usecache=usecache&&(cachesize>0);
        useDB=Boolean.valueOf(cfg.params.getProperty("database")).booleanValue();
        usequeue=Boolean.valueOf(cfg.params.getProperty("queue")).booleanValue();

        logFileName=cfg.params.getProperty("logfile");
        try
          { logFile=new PrintWriter(new FileWriter(logFileName),true); }
        catch (IOException e)
          { System.out.println("Error: unable to open logging file ("+logFileName+")");
            log=false; }

      /*** Initialize and start ***/
        initialize();
        listen();
    }
     

    public void initialize()
    {
        serverMessage(versionTag+"\n");
	    serverMessage("Initializing Pegasus internal services ...\n");

        try
        {
            svrSock=new ServerSocket(svrPort);
    	    svrHost=new String("Server (current) Address - "+svrSock.getInetAddress().getLocalHost().getHostName()+":"+svrPort);	    
        }
        catch (UnknownHostException e)
        {
            svrHost=new String("Server (current) Address - (localhost)");
        }     
        catch (IOException e)
        {
            serverMessage("Error: unable to open server (local) socket.");
            shutDown(-2);
        }           

        if (usecache)
        {
            cache=new ProfileCache();
            cache.setLimit(cachesize);
            serverMessage("Cache buffers initialized");
        }
        
        if (useDB)
        {
            users=new UserDatabase();
            if (users.readDatabase())
                serverMessage("User database initialized");
            else
                serverMessage("Error: User database init: failed");
        }

        if (usequeue)
        {
            queue=new ClientQueue();
            serverMessage("Client queue initialized");
        }
        
        serverMessage("initializing protein prediction server...");
        predictor=new ProtPredServer();
        if (!(predictor.readParams() && predictor.initialize()))
        {
            serverMessage("Error: unable to start protein prediction services.");
            shutDown(-3);
        }           

        serverMessage("initializing mailing services...");
        mailer=new MailServer();
        if (!mailer.initialize())
        {
            serverMessage("Error: unable to start mailing services.");
            shutDown(-4);
        }           

    /*... initialize local variables ...*/

        cAcc=0;  cHold=0;  cRun=0;
        currDate=new Date();

        memTotal=Runtime.getRuntime().totalMemory();
        memFree=Runtime.getRuntime().freeMemory();
        
        serverRunning=true;

        serverMessage("\nFinished: all internal services are now active.");

        updateInfo();
        serverMessage("Pegasus server initialized!");
    }
    

    public void listen()
    {
        int     lev;
        CliRegistry cliReg;
        CliProcess  cli;
        String  str0, str;
        // BufferedReader  sockinp;
        // PrintWriter     sockout;
        DataInputStream    sockinp;
        PrintStream        sockout;

        
        while (true)
        {
            try 
            {
                serverMessage("(Idle)");
                cliSock=svrSock.accept();

                if (serverRunning)
                {
                    cAcc++;  cRun++;

                    serverMessage("Client #"+cAcc+": accepted");

                  /*** receive client registry ***/
                    try
                    {
                        // sockinp=new BufferedReader(new InputStreamReader(cliSock.getInputStream()),1000);
                        // sockout=new PrintWriter(cliSock.getOutputStream(),true);
                        sockinp=new DataInputStream(cliSock.getInputStream());
                        sockout=new PrintStream(cliSock.getOutputStream(),true);

                        str0=sockinp.readLine();
                        str=new String(str0.substring(str0.indexOf(":")+1));

                        if (str0.startsWith(Messages.commandStart))
                        {
                            if (str.equals(Messages.rcmdAck))
                            {
                                serverMessage("Client #"+cAcc+": remote command: 'SendACK'");
                                sockout.println("server: ACKNOWLEDGE");
                            }
                            else if (str.equals(Messages.rcmdInfo))
                            {
                                serverMessage("Client #"+cAcc+": remote command: 'SendINFO'");
                                sockout.println("server: STATUS"+infoString());
                            }
                            else if (str.equals(Messages.rcmdQuit))
                            {
                                serverMessage("Client #"+cAcc+": remote command: 'DoQUIT'");
                                sockout.println("server: SHUTDOWN");
                                try { sockinp.close(); sockout.close(); cliSock.close(); }
                                catch (Exception e0)  {}
                                shutDown(0);
                            }
                            else
                            {
                                serverMessage("Client #"+cAcc+": failed");
                                sockout.println("server: unknown command");
                                try { sockinp.close(); sockout.close(); cliSock.close(); }
                                catch (Exception e0)  {}
                                continue;
                            }
                    
                            serverMessage("Client #"+cAcc+": completed");
                            try { sockinp.close(); sockout.close(); cliSock.close(); }
                            catch (Exception e0)  {}
                            continue;
                        }
                        else if (!str0.startsWith(Messages.requestStart))
                        {
                            serverMessage("Error: unrecognized data header");
                            sockout.println("server: unrecognized data header");
                            serverMessage("Client #"+cAcc+": failed");
                            try { sockinp.close(); sockout.close(); cliSock.close(); }
                            catch (Exception e0)  {}
                            continue;
                        }

                        cliReg=new CliRegistry(str);
                        serverMessage("Client #"+cAcc+": registry received");
                        if (usequeue)  lev=queue.defPriority;
                        else  lev=Thread.NORM_PRIORITY;
                        //System.out.println("new client: "+cliReg.toString());

                        if (useDB)
                        {
                            if (users.containsUser(cliReg.email,cliReg.userid))
                            {
                                try
                                {
                                    lev=users.queryUser(cliReg.email,cliReg.userid);
                                    serverMessage("Client #"+cAcc+": registered user (lev="+lev+")");
                                }
                                catch (IllegalAccessException e)
                                  { //System.out.println("Error: unable to access users database");
                                    try { sockinp.close(); sockout.close(); cliSock.close(); }
                                    catch (Exception e0)  {}
                                    continue; }
                            }
                            //else
                            //    System.out.println("user NOT found: "+cliReg.email+":"+cliReg.userid+"_"+lev);
                        }
                    }
                    catch (IOException e4)
                      { serverMessage("Error: unable to read registry");
                        try  { cliSock.close(); }
                        catch (Exception e0)  {}
                        continue; }

            /*** client notification ***

                  //----- use specific datagram socket to send notification -----
                    InetAddress cliaddr=cliSock.getInetAddress();
                    int cliport=cliSock.getPort();
                    //try  { cliSock.close(); cliSock=null; }
                    //catch (IOException e) { serverMessage("Error closing init socket"); }

                    byte buff=new byte[1];  buff[0]=0;
                    DatagramPacket dgpack=new DatagramPacket(buff,buff.length,cliaddr,cliport+1);
                    try
                    {
                        DatagramSocket dgsock=new DatagramSocket(8885);
                        dgsock.send(dgpack);
                        //System.out.println("notification sent");
                    }
                    catch (IOException e)
                    {
                        //System.out.println("datagramsocket failed");
                        continue;
                    }
                    
                  //----- or open a new (client) socket to send notification -----
                    try
                    {
                        serverMessage("Client #"+cAcc+": trying to notify ("+cliReg.hostName+":"+cliReg.hostPort+")");
                        cliSock=new Socket(cliReg.hostName,cliReg.hostPort.intValue()+1);
                    }
                    catch (UnknownHostException e)
                      { serverMessage("Error: client #"+cAcc+" is unreachable (unknown host)");
                        continue; }
                    catch (IOException e)
                      { serverMessage("Error: client #"+cAcc+" is unreachable (IO error)");
                        //serverMessage(e.toString());
                        continue; }
            ****************************/

                  /*... start client process ...*/

                    // ERROR: closing the streams the socket closes too!!!!
                    //try { sockinp.close(); sockout.close(); }  catch (IOException e)  {}

                    cli=new CliProcess(this,cliSock,cAcc,cliReg,lev);
                    try
                      { //System.out.println("waiting client #"+cAcc);
                        cli.cliProc.join(); }
                    catch (InterruptedException e4)
                      { serverMessage("Client #"+cAcc+": aborted (interrupted)"); }
                    
                    cRun--;
                }

                try  { cliSock.close(); }
                catch (Exception e0)  {}
            }
            catch (IOException e2)
            { serverMessage("Error: unable to accept client"); }
        }        
    }


    public void shutDown( int exitvalue )
    {
        serverMessage("Entering termination process (QUIT SIG)");

        serverMessage("    terminating internal services...");
        // .......
        
        serverMessage("    terminating logging services...");
        serverMessage("Pegasus server is down (on since: "+currDate+").");
        cfg.writeParamFile();
        if ((log)&&(logFile!=null))  logFile.close();

        System.exit(exitvalue);
    }
    

    public String infoString()
    {
        StringBuffer sbuff=new StringBuffer();
        
        sbuff.append("$"+versionTag+"$");
        sbuff.append("Server started at:  "+currDate.toString()+"$");
        sbuff.append(svrHost+"$");
        sbuff.append("Operating system:  "+System.getProperty("os.name","(unknown)")+"$");
        sbuff.append("System architecture:  "+System.getProperty("os.arch","(unknown)")+"$");
        sbuff.append("Java version:  "+System.getProperty("java.version","(unknown)")+"$");
        sbuff.append("Java class version:  "+System.getProperty("java.class.version","(unknown)")+"$");
        sbuff.append("Current status:  ");
        if (serverRunning)  sbuff.append("(running)"+"$");
        else  sbuff.append("(stopped)"+"$");
        sbuff.append("Clients:  Accepted="+cAcc+" , OnHold="+cHold+" , Running="+cRun+"$");
        sbuff.append("Memory:  Total="+Runtime.getRuntime().totalMemory());
        sbuff.append(" , Free="+Runtime.getRuntime().freeMemory()+"$");
        if (usecache)  sbuff.append("Server is using cache (buffers="+cachesize+")"+"$");
        if (usequeue)  sbuff.append("Server is using client queue"+"$");
        if (useDB)  sbuff.append("Server is using users database"+"$");
        sbuff.append(predictor.getInfo().replace('\n','$'));
        sbuff.append(mailer.getInfo().replace('\n','$'));

        return(sbuff.toString());
    }
    

    public void updateInfo()
    {
       if (tty)
       {
           System.out.println("\n"+versionTag+"\n");
           System.out.println("Server started at:  "+currDate.toString());
           System.out.println(svrHost);
           System.out.println("Operating system:  "+System.getProperty("os.name","(unknown)"));
           System.out.println("System architecture:  "+System.getProperty("os.arch","(unknown)"));
           System.out.println("Java version:  "+System.getProperty("java.version","(unknown)"));
           System.out.println("Java class version:  "+System.getProperty("java.class.version","(unknown)"));
           System.out.print("Current status:  ");
           if (serverRunning)  System.out.println("(running)");
           else  System.out.println("(stopped)");
           System.out.println("Clients:  Accepted="+cAcc+" , OnHold="+cHold+" , Running="+cRun);
           System.out.print("Memory:  Total="+Runtime.getRuntime().totalMemory());
           System.out.println(" , Free="+Runtime.getRuntime().freeMemory());
           if (usecache)  System.out.println("Server is using cache (buffers="+cachesize+")");
           if (usequeue)  System.out.println("Server is using client queue");
           if (useDB)  System.out.println("Server is using users database");
           System.out.print(predictor.getInfo());  System.out.flush();
           System.out.println(mailer.getInfo());  System.out.flush();
           // System.out.println();
       }

        if ((log)&&(logFile!=null))
        {
            if (!logFile.checkError())
            {
               logFile.println("\n"+versionTag+"\n");
               logFile.println("Server started at:  "+currDate.toString());
               logFile.println(svrHost);
               logFile.println("Operating system:  "+System.getProperty("os.name","(unknown)"));
               logFile.println("System architecture:  "+System.getProperty("os.arch","(unknown)"));
               logFile.println("Java version:  "+System.getProperty("java.version","(unknown)"));
               logFile.println("Java class version:  "+System.getProperty("java.class.version","(unknown)"));
               logFile.print("Current status:  ");
               if (serverRunning)  logFile.println("(running)");
               else  logFile.println("(stopped)");
               logFile.println("Clients:  Accepted="+cAcc+" , OnHold="+cHold+" , Running="+cRun);
               logFile.print("Memory:  Total="+Runtime.getRuntime().totalMemory());
               logFile.println(" , Free="+Runtime.getRuntime().freeMemory());
               if (usecache)  logFile.println("Server is using cache (buffers="+cachesize+")");
               if (usequeue)  logFile.println("Server is using client queue");
               if (useDB)  logFile.println("Server is using users database");
               logFile.print(predictor.getInfo());  System.out.flush();
               logFile.print(mailer.getInfo());  System.out.flush();
               // logFile.println();
            }
        }
    }
    

    public synchronized void serverMessage( String msg )
    {
        if (tty)  System.out.println(msg);

        if ((log)&&(logFile!=null))
        {
            if (!logFile.checkError())  logFile.println(msg);
        }
    }
    

}


     
