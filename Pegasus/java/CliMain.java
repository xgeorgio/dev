import java.applet.Applet;
import java.awt.*;
import java.util.*;
import java.io.*;
import java.net.*;


public class CliMain extends Applet
{
    protected CardLayout    cards;
    protected Panel     card1, card2, card3;
    protected GridBagLayout     gridbag;
    protected GridBagConstraints    c;
    protected Panel     p0, p1, p2;
    protected Label     Lmail, Luser, Lproc, Lwin, Llim;
    protected Label     Lpred, Lseq, Separ, title;
    protected Label     Lppred, Lcfreq, Lccorrl, Lcnotes;

    protected TextField     Vwin1, Vwin3, Vlo1, Vlo3, Vhi1, Vhi3;
    protected TextField     Vmail1, Vmail3, Vuser1, Vuser3, Vproc3;
    protected TextField     Vstat, Vmesg, Vpred;
    protected TextArea      Vseq1, Vseq3;
    protected TextArea      Vppred, Vcfreq, Vccorrl, Vcnotes;
    protected Checkbox      Vusewin;

    protected Choice    Vproc1;
    protected Button    btnSend, btnReset;
    protected Label     Lstat;
    protected Button    btnCancel, btnRetry, btnClose;

    protected Label         Lprog;
    protected TextField     Vprog;
    // protected ProgressBar   Vbar;
    
    protected ProteinChars    pchars;
    protected DelimitChars    dchars;
    protected String    svrHost, svrResults;
    protected int       svrPort;
    protected Socket    svrSock;

    // protected BufferedReader    svrRecv;
    // protected PrintWriter       svrSend;
    protected DataInputStream   svrRecv;
    protected PrintStream       svrSend;

    protected boolean   waitforans;

    public final static String versionTag="Pegasus Applet Client, version 2.0 (R981005) - Harris Georgiou (c) 1998.";
    protected static String requestStart="Pegasus_Application_Protocol_V2.01-REQUESTDATA";


    public void init()
    {              

/*** Card 1 Setup ***/

        p0=new Panel();
        p0.setLayout(new FlowLayout());
        
        title=new Label(versionTag);
        title.setAlignment(title.CENTER);
        p0.add("Center",title);
        
        
        p1=new Panel();
        gridbag=new GridBagLayout();
        c=new GridBagConstraints();
        p1.setLayout(gridbag);
        
        c.ipadx=5;  c.ipady=5;
        c.insets=new Insets(5,5,5,5);
        
        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lmail=new Label("Email:");
        gridbag.setConstraints(Lmail,c);
        p1.add(Lmail);
        
        c.anchor=GridBagConstraints.WEST;
        Vmail1=new TextField("<Enter your EMAIL address here>",25);
        gridbag.setConstraints(Vmail1,c);
        p1.add(Vmail1);         

        c.anchor=GridBagConstraints.EAST;
        Luser=new Label("User-ID:");
        gridbag.setConstraints(Luser,c);
        p1.add(Luser);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vuser1=new TextField("",10);
        Vuser1.setEchoCharacter('*');
        gridbag.setConstraints(Vuser1,c);
        p1.add(Vuser1);         

        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lproc=new Label("Process Type:");
        gridbag.setConstraints(Lproc,c);
        p1.add(Lproc);
        
        c.anchor=GridBagConstraints.WEST;
        Vproc1=new Choice();
        Vproc1.addItem("Interactive (on-line)");
        Vproc1.addItem("Batch mode (mail)");
        Vproc1.addItem("Both (on-line and mail)");
        gridbag.setConstraints(Vproc1,c);
        p1.add(Vproc1);         

        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.EAST;
        Vusewin=new Checkbox("Make windowed analysis");
        Vusewin.setState(true);
        gridbag.setConstraints(Vusewin,c);
        p1.add(Vusewin);         

        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lwin=new Label("Window size:");
        gridbag.setConstraints(Lwin,c);
        p1.add(Lwin);
        
        c.anchor=GridBagConstraints.WEST;
        Vwin1=new TextField("0",3);
        gridbag.setConstraints(Vwin1,c);
        p1.add(Vwin1);         

        c.anchor=GridBagConstraints.EAST;
        Llim=new Label("%Thresholds(lo,hi):");
        gridbag.setConstraints(Llim,c);
        p1.add(Llim);
        
        c.anchor=GridBagConstraints.WEST;
        Vlo1=new TextField("10",2);
        gridbag.setConstraints(Vlo1,c);
        p1.add(Vlo1);         

        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vhi1=new TextField("10",2);
        gridbag.setConstraints(Vhi1,c);
        p1.add(Vhi1);         

        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lseq=new Label("Sequence:");
        gridbag.setConstraints(Lseq,c);
        p1.add(Lseq);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vseq1=new TextArea("<Enter the SEQUENCE to be analyzed here>",5,55);
        gridbag.setConstraints(Vseq1,c);
        p1.add(Vseq1);         


        p2=new Panel();
        p2.setLayout(new FlowLayout());
        
        btnSend=new Button("   Send Request   ");
        btnReset=new Button("    Reset Form    ");
        Separ=new Label("     ");
        p2.add("Left",btnSend);
        p2.add("Center",Separ);
        p2.add("Right",btnReset);
             
        card1=new Panel();
        card1.setLayout(new BorderLayout());
        card1.add("North",p0);
        card1.add("Center",p1);
        card1.add("South",p2);
     

/*** Card 2 Setup ***/

        p0=new Panel();
        p0.setLayout(new FlowLayout());
        
        title=new Label(versionTag);
        title.setAlignment(title.CENTER);
        p0.add("Center",title);
        
        
        p1=new Panel();
        gridbag=new GridBagLayout();
        c=new GridBagConstraints();
        p1.setLayout(gridbag);
        
        c.ipadx=1;  c.ipady=1;
        c.insets=new Insets(5,5,5,5);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Lstat=new Label("Status:");
        gridbag.setConstraints(Lstat,c);
        p1.add(Lstat);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vstat=new TextField("",50);
        Vstat.setEditable(false);
        gridbag.setConstraints(Vstat,c);
        p1.add(Vstat);
         
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.CENTER;
        Separ=new Label("");
        gridbag.setConstraints(Separ,c);
        p1.add(Separ);

        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        /*** last colon is needed to allocate space for all characters ***/
        Lprog=new Label("Progress:                               .");
        // Lprog=new Label("Progress:");
        gridbag.setConstraints(Lprog,c);
        p1.add(Lprog);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vprog=new TextField("",50);
        Vprog.setEditable(false);
        gridbag.setConstraints(Vprog,c);
        p1.add(Vprog);
        // Vbar=new ProgressBar();
        // gridbag.setConstraints(Vbar,c);
        // p1.add(Vbar);
         
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.CENTER;
        Separ=new Label("");
        gridbag.setConstraints(Separ,c);
        p1.add(Separ);

        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vmesg=new TextField("",50);
        Vmesg.setEditable(false);
        gridbag.setConstraints(Vmesg,c);
        p1.add(Vmesg);
         

        p2=new Panel();
        p2.setLayout(new FlowLayout());
        
        btnCancel=new Button("   Cancel   ");
        Separ=new Label("     ");        
        btnRetry=new Button("   Retry   ");
        btnRetry.disable();
        p2.add("Left",btnRetry);
        p2.add("Center",Separ);
        p2.add("Right",btnCancel);       
     
        card2=new Panel();
        card2.setLayout(new BorderLayout());
        card2.add("North",p0);
        card2.add("Center",p1);
        card2.add("South",p2);


/*** Card 3 Setup ***/

        p0=new Panel();
        p0.setLayout(new FlowLayout());
        
        title=new Label(versionTag);
        title.setAlignment(title.CENTER);
        p0.add("Center",title);
        
        
        p1=new Panel();
        gridbag=new GridBagLayout();
        c=new GridBagConstraints();
        p1.setLayout(gridbag);
        
        c.ipadx=3;  c.ipady=3;
        c.insets=new Insets(5,5,5,5);
        
        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lmail=new Label("E-mail:");
        gridbag.setConstraints(Lmail,c);
        p1.add(Lmail);
        
        c.anchor=GridBagConstraints.WEST;
        Vmail3=new TextField(Vmail1.getText(),25);
        Vmail3.setEditable(false);
        gridbag.setConstraints(Vmail3,c);
        p1.add(Vmail3);
         
        c.anchor=GridBagConstraints.EAST;
        Luser=new Label("User type:");
        gridbag.setConstraints(Luser,c);
        p1.add(Luser);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vuser3=new TextField("",12);
        Vuser3.setEditable(false);
        gridbag.setConstraints(Vuser3,c);
        p1.add(Vuser3);
        
        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lproc=new Label("Process Type:");
        gridbag.setConstraints(Lproc,c);
        p1.add(Lproc);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vproc3=new TextField(Vproc1.getSelectedItem(),25);
        Vproc3.setEditable(false);
        gridbag.setConstraints(Vproc3,c);
        p1.add(Vproc3);         

        c.gridwidth=1;
        c.anchor=GridBagConstraints.EAST;
        Lproc=new Label("Window size:");
        gridbag.setConstraints(Lproc,c);
        p1.add(Lproc);
        
        c.anchor=GridBagConstraints.WEST;
        Vwin3=new TextField(Vwin1.getText(),3);
        Vwin3.setEditable(false);
        gridbag.setConstraints(Vwin3,c);
        p1.add(Vwin3);         

        c.anchor=GridBagConstraints.EAST;
        Llim=new Label("%Thresholds(lo,hi):");
        gridbag.setConstraints(Llim,c);
        p1.add(Llim);
        
        c.anchor=GridBagConstraints.WEST;
        Vlo3=new TextField(Vlo1.getText(),4);
        Vlo3.setEditable(false);
        gridbag.setConstraints(Vlo3,c);
        p1.add(Vlo3);         

        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vhi3=new TextField(Vhi1.getText(),4);
        Vhi3.setEditable(false);
        gridbag.setConstraints(Vhi3,c);
        p1.add(Vhi3);         

        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lseq=new Label("Sequence:");
        gridbag.setConstraints(Lseq,c);
        p1.add(Lseq);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vseq3=new TextArea("",1,55);
        Vseq3.setEditable(false);
        gridbag.setConstraints(Vseq3,c);
        p1.add(Vseq3);
                  
        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lpred=new Label("Prediction:");
        gridbag.setConstraints(Lpred,c);
        p1.add(Lpred);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vpred=new TextField("",35);
        Vpred.setEditable(false);
        gridbag.setConstraints(Vpred,c);
        p1.add(Vpred);
         
        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lppred=new Label("Profile:");
        gridbag.setConstraints(Lppred,c);
        p1.add(Lppred);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vppred=new TextArea("",1,55);
        Vppred.setEditable(false);
        gridbag.setConstraints(Vppred,c);
        p1.add(Vppred);

        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lcfreq=new Label("Analysis:");
        gridbag.setConstraints(Lcfreq,c);
        p1.add(Lcfreq);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vcfreq=new TextArea("",1,55);
        Vcfreq.setEditable(false);
        gridbag.setConstraints(Vcfreq,c);
        p1.add(Vcfreq);

        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lccorrl=new Label("Correlations:");
        gridbag.setConstraints(Lccorrl,c);
        p1.add(Lccorrl);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vccorrl=new TextArea("",1,55);
        Vccorrl.setEditable(false);
        gridbag.setConstraints(Vccorrl,c);
        p1.add(Vccorrl);

        c.gridwidth=1;
        c.anchor=GridBagConstraints.NORTHEAST;
        Lcnotes=new Label("Notes:");
        gridbag.setConstraints(Lcnotes,c);
        p1.add(Lcnotes);
        
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.anchor=GridBagConstraints.WEST;
        Vcnotes=new TextArea("",1,55);
        Vcnotes.setEditable(false);
        gridbag.setConstraints(Vcnotes,c);
        p1.add(Vcnotes);


        p2=new Panel();
        p2.setLayout(new FlowLayout());
        
        btnClose=new Button("   Close   ");
        p2.add("Center",btnClose);
        

        card3=new Panel();     
        card3.setLayout(new BorderLayout());
        card3.add("North",p0);
        card3.add("Center",p1);
        card3.add("South",p2);


/*** Global Form Setup ***/

        setLayout(new CardLayout());
        add("card1",card1);
        add("card2",card2);
        add("card3",card3);


/*** Global Variables Initialization ***/

        pchars=new ProteinChars();
        dchars=new DelimitChars();
	        
             
/*** Start the form presentation ***/
                
        switchToCard1("Ready for new request");
   }
   
   public void switchToCard1( String status )
   {
        ((CardLayout)this.getLayout()).show(this,"card1");
        if (status!="") getAppletContext().showStatus(status);
   }
          
   public void switchToCard2( String status )
   {
       Vstat.setText("");
       // Vbar.updateValue(0); Vbar.resetBar();
       ((CardLayout)this.getLayout()).show(this,"card2");
       if (status!="") getAppletContext().showStatus(status);
   }
          
   public void switchToCard3( String status )
   {
       //Vname3.setText(Vname1.getText());
       Vmail3.setText(Vmail1.getText());
       Vwin3.setText(Vwin1.getText());
       Vlo3.setText(Vlo1.getText());
       Vhi3.setText(Vhi1.getText());
       Vseq3.setText(Vseq1.getText().toUpperCase());
       ((CardLayout)this.getLayout()).show(this,"card3");
       if (status!="") getAppletContext().showStatus(status);
   }
          
   public String getSequence()
   {
       String str=new String(Vseq1.getText().toUpperCase());
       return(dchars.packString(str));
   }
   
   public boolean checkFormData()
   {
       String str;
       boolean  valid;
       int      ival;
       

     /*** form completeness checks ***/
       if ( ((Vproc1.getSelectedIndex()==1)||(Vproc1.getSelectedIndex()==2))
            && (Vmail1.getText().equals("")) )
       {
           // OKDialog.exec("Error","You must give a valid e-mail address");
           getAppletContext().showStatus("Error: You must give a valid e-mail address");
           return(false);
       }
       
     /*** sequence validation ckecks ***/
       Vseq1.setText(Vseq1.getText().toUpperCase());
       str=getSequence();
       
       if (str.equals(""))
       {
           // OKDialog.exec("Error","You must give a valid protein sequence.");
           getAppletContext().showStatus("Error: You must give a valid protein sequence");
           return(false);
       }
      
       if (!pchars.isValid(str))
       {
           // OKDialog.exec("Error","Invalid protein sequence data");
           getAppletContext().showStatus("Error: Invalid protein sequence data");
           return(false);
       }

     /*** analysis parameters ckecks ***/
       str=Vwin1.getText();
       if (str=="")  valid=false;
       else
       {
           try
           {
               ival=Integer.parseInt(str);
               valid=(ival>0);

               if ((valid)&&(!Vusewin.getState())&&(ival<Vseq1.getText().length()+1))
                   winAnalysisState(false);
               else if ((Vusewin.getState())&&(ival>Vseq1.getText().length()))
                   winAnalysisState(false);
           }
           catch (NumberFormatException e)
             { valid=false; }
       }

       if (!valid)
       {
           // OKDialog.exec("Error","Invalid analysis window size");
           getAppletContext().showStatus("Error: Invalid analysis window size");
           return(false);
       }

       str=Vlo1.getText();
       if (str=="")  valid=false;
       else
       {
           try
             { ival=Integer.parseInt(str);
               valid=((ival>=0)&&(ival<=100)); }
           catch (NumberFormatException e)
             { valid=false; }
       }

       if (!valid)
       {
           // OKDialog.exec("Error","Invalid analysis lower rejection %");
           getAppletContext().showStatus("Error: Invalid analysis lower rejection %");
           return(false);
       }

       str=Vhi1.getText();
       if (str=="")  valid=false;
       else
       {
           try
             { ival=Integer.parseInt(str);
               valid=((ival>=0)&&(ival<=100)); }
           catch (NumberFormatException e)
             { valid=false; }
       }

       if (!valid)
       {
           // OKDialog.exec("Error","Invalid analysis lower rejection %");
           getAppletContext().showStatus("Error: Invalid analysis lower rejection %");
           return(false);
       }


       return(true);
   }
   

   public boolean makeRequest()
   {
       DataProfile  reqProf;
       boolean      res;
       int          localport;
       String       rcvstr;
       

       switchToCard2("Running: Executing request");
       updateProgress(0);  


     /*** preparing request data (data profile) ***/

       reqProf=new DataProfile();
       reqProf.header.put("client",versionTag);
       reqProf.inpseq.put("sequence",getSequence());
       reqProf.header.put("email",Vmail1.getText());
       reqProf.header.put("userid",Vuser1.getText());
       reqProf.header.put("proc",Vproc1.getSelectedItem());
       reqProf.header.put("sendmail",""+(Vproc1.getSelectedIndex()!=0));
       reqProf.header.put("sendsock",""+(Vproc1.getSelectedIndex()!=1));
       reqProf.params.put("winsize",Vwin1.getText());
       reqProf.params.put("lorejlim",Vlo1.getText());
       reqProf.params.put("hirejlim",Vhi1.getText());

       if (Vproc1.getSelectedIndex()!=1)  waitforans=true;
       else  waitforans=false;
       
       updateProgress(5);


    /*** Connecting with server ***/      

       Vstat.setText("Initializing connection...");       
       Vmesg.setText("Identifing server connection...");
       svrHost=new String(getCodeBase().getHost());
       svrPort=Integer.parseInt(getParameter("port"));
       // svrPort=8881;
       
       try
       {
           Vmesg.setText("Opening socket connection... ("+svrHost+":"+svrPort+")");
           svrSock=new Socket(svrHost,svrPort);
           localport=svrSock.getLocalPort();

           Vmesg.setText("Opening socket streams...");
           // svrRecv=new BufferedReader(new InputStreamReader(svrSock.getInputStream()),1000);
           // svrSend=new PrintWriter(svrSock.getOutputStream(),true);
           svrRecv=new DataInputStream(svrSock.getInputStream());
           svrSend=new PrintStream(svrSock.getOutputStream(),true);

           updateProgress(15);
       }
       catch (IOException e10)
       {
           Vstat.setText("Stopped: I/O error");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Cannot initialize connection with server");
           // OKDialog.exec("Connection Error","Cannot initialize connection\nwith server.");           

           try
           {
               Vmesg.setText("Closing socket streams...");
               svrRecv.close();
               svrSend.close();
               Vmesg.setText("Closing socket connection...");
               svrSock.close();
           }
           catch (Exception e11)
           {}
           finally
           {
             /*** This should not be executed if connection failed!!! ***/
               Vmesg.setText("");
               return(false);
           }
       }


     /*** Sending request registry ***/

       Vstat.setText("Sending request registry to server...");       
       Vmesg.setText("Waiting server confirmation...");

       StringBuffer sbuff=new StringBuffer();
       String   str;
       
       sbuff.append(requestStart+":");
       try
         { str=svrSock.getInetAddress().getLocalHost().getHostName(); }
       catch (UnknownHostException e)
         { str=new String("(localhost)"); }
       sbuff.append(str);
       sbuff.append(":"+localport+":");
       sbuff.append(getSequence()+":");
       if (Vmail1.getText().equals(""))  sbuff.append("@:");
       else  sbuff.append(Vmail1.getText()+":");
       if (Vuser1.getText().equals(""))  sbuff.append("@:");
       else  sbuff.append(Vuser1.getText()+":");
       sbuff.append(Vproc1.getSelectedItem()+":");
       sbuff.append(Vwin1.getText()+":");
       sbuff.append(Vlo1.getText()+":");
       sbuff.append(Vhi1.getText());
       str=sbuff.toString();

       //System.out.println("registry: "+str);
       svrSend.println(str);
       res=(!svrSend.checkError());
       if (!res)
       {
           Vstat.setText("Stopped: I/O error");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Unable to send registry data");
           // OKDialog.exec("Connection Error","Unable to send registry data.");          

           try
           {
               Vmesg.setText("Closing socket streams...");
               svrRecv.close();
               svrSend.close();
               Vmesg.setText("Closing socket connection...");
               svrSock.close();

               return(false);
           }
           catch (Exception e21)
           {}
           finally
           {
             /*** This should not be executed if connection failed!!! ***/
               Vmesg.setText("");
               return(false);
           }
       }
        
       updateProgress(25);


/*** wait server authorization ***

        InetAddress cliaddr=svrSock.getInetAddress();
        int  cliport=svrSock.getLocalPort();
        //try  { svrSock.close(); svrSock=null; }
        //catch (IOException e)  { System.out.println("ERROR: unable to close init socket"); }

      //----- use specific datagram socket for notification -----
        byte buff[]=new byte[1]; buff[0]=0;
        DatagramPacket dgpack=new DatagramPacket(buff,buff.length);
        try
        {
            DatagramSocket dgsock=new DatagramSocket(cliport+1);
            dgsock.receive(dgpack);
            System.out.println("notification received");
        }
        catch (IOException e)
        {
            System.out.println("datagramsocket failed");
            return(false);
        }
                    
      //----- or use a server socket for notification -----
       try
       {
           Vmesg.setText("Waiting server notification... (:"+localport+")");
           ServerSocket svrCSock=new ServerSocket(localport+1);
           svrSock=svrCSock.accept();
       }
       catch (IOException e)
       {
           Vstat.setText("Stopped: I/O error");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Cannot initialize connection with server");
           System.out.println(e.toString());
           // OKDialog.exec("Connection Error","Cannot initialize connection\nwith server.");
       }
       
       try
       {
           Vmesg.setText("Opening socket connection... ("+svrHost+":"+svrPort+")");

           Vmesg.setText("Opening new socket streams...");
           svrRecv=new DataInputStream(svrSock.getInputStream());
           svrSend=new PrintStream(svrSock.getOutputStream());

           updateProgress(15);
       }
       catch (IOException e10)
       {
           Vstat.setText("Stopped: I/O error");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Cannot initialize connection with server");
           // OKDialog.exec("Connection Error","Cannot initialize connection\nwith server.");           

           try
           {
               Vmesg.setText("Closing socket connection...");
               svrSock.close();
               Vmesg.setText("Closing socket streams...");
               svrRecv.close();
               svrSend.close();
           }
           catch (Exception e11)
           {}
           finally
           {
               Vmesg.setText("");
               return(false);
           }
       }
***************************/


     /*** Sending request ***/      

       Vstat.setText("Sending request to server...");       
       //System.out.println("REQUEST DATA:  \n"+reqProf.toString()+"\n");
       Vmesg.setText("Waiting server results...");

       res=reqProf.sendProfile(svrSock);
       //svrSend.println(reqProf.toTransString());
       //res=(!svrSend.checkError());
       if (!res)
       {
           //System.out.println("client send: FAILURE");
           Vstat.setText("Stopped: I/O error");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Unable to send request data");
           // OKDialog.exec("Connection Error","Unable to send request data.");          

           try
           {
               Vmesg.setText("Closing socket streams...");
               svrRecv.close();
               svrSend.close();
               Vmesg.setText("Closing socket connection...");
               svrSock.close();

               return(false);
           }
           catch (Exception e21)
           {}
           finally
           {
             /*** This should not be executed if connection failed!!! ***/
               Vmesg.setText("");
               return(false);
           }
       }

       // System.out.println("client send: SUCCESS");
       updateProgress(50);


       if (!waitforans)
       {
           updateProgress(100);
           Vstat.setText("Request sent.");
           Vmesg.setText("");
           return(true);
       }
       
           
     /*** Receiving results ***/      

       Vstat.setText("Request sent.");
       Vmesg.setText("Waiting server results...");
        
       try
       {
           res = reqProf.recvProfile(svrSock);
           //if (!res)  System.out.println("error #1");

           //rcvstr=svrRecv.readLine();
           //if (!res)  System.out.println("error #2");
           //res = res && reqProf.transString(rcvstr);
           //if (!res)  System.out.println("error #3");
       }
       catch (Exception e)
       {
           res=false;
           // System.out.println("error #0: "+e.toString()+"\n");
           // e.printStackTrace(System.out);
       }
           
       if (!res)
       {
           System.out.println("client recv: FAILURE");
           // System.out.println("server sent:\n"+reqProf.toString()+"\n");
           Vstat.setText("Stopped: Connection refused by server");
           Vmesg.setText("");
           updateProgress(-1);
           getAppletContext().showStatus("Connection Error: Connection refused by server.");
           // OKDialog.exec("Connection Error","Server has refused connection.");          
           return(false);
       }
       else
       {
           // System.out.println("client recv: SUCCESS");
           // System.out.println("server sent:\n"+reqProf.toString()+"\n");
           Vstat.setText("Results received.");
           Vmesg.setText("Analyzing and formatting data profile...");
           updateProgress(80);

           setFormFields(reqProf);
           updateProgress(95);
           
           String svrflag=(String)reqProf.header.get("outcome");
           if (svrflag.equals("success"))
           {
               getAppletContext().showStatus("Request completed");
           }
           else
           {
               getAppletContext().showStatus(svrflag);
               updateProgress(-1);
               res=false;
           }
       }
       
       // System.out.println("RESULTS DATA:  \n"+reqProf.toString()+"\n");
       Vstat.setText("Process completed.");
       Vmesg.setText("Done");
       updateProgress(100);
       
       try
       {
           svrSock.close();
           svrRecv.close();
           svrSend.close();
       }
       catch (Exception e90)
       {}
       finally
       {
           // switchToCard3();
           return(true);
       }
   }
       

   public void setFormFields( DataProfile prf )
   {
       StringTokenizer tok1, tok2;
       StringBuffer    sbuff;
       String   str;
       int      i, c, lovalue, hivalue, freqs[]=new int[48];

       
     /*** user type ***/
       Vuser3.setText((String)prf.header.get("usertype"));

     /*** total (and window) final prediction(s) ***/
       Vpred.setText("Total: "+(String)prf.totalp.get("prediction"));
       str=(String)prf.windowp.get("prediction");
       if (!str.equals(""))
           Vpred.setText(Vpred.getText()+"  ,  Window: "+str);

     /*** prediction profile (analysis) ***/
       tok1=new StringTokenizer((String)prf.windowp.get("winseqs"),"/");
       tok2=new StringTokenizer((String)prf.windowp.get("results"),"/");
       sbuff=new StringBuffer();
       if ((tok1.hasMoreTokens())||(tok2.hasMoreTokens()))
           sbuff.append("Windows and Predictions:\n");
       while ((tok1.hasMoreTokens())||(tok2.hasMoreTokens()))
           sbuff.append("\t"+tok1.nextToken()+" : "+tok2.nextToken()+"\n");
       Vppred.setText(sbuff.toString());
       
     /*** class frequencies ***/
       tok1=new StringTokenizer((String)prf.clsfreq.get("selections"),"/");
       sbuff=new StringBuffer();
       if (tok1.hasMoreTokens())
           sbuff.append("Class selections:\n\t");
       while (tok1.hasMoreTokens())
       {
           str=tok1.nextToken();
           sbuff.append(str);
           if (tok1.hasMoreTokens())  sbuff.append(",");
       }
       Vcfreq.setText(sbuff.toString()+"\n");

       //for ( i=0; i<48; i++ )  freqs[i]=0;
       tok1=new StringTokenizer((String)prf.clsfreq.get("freqs"),"/");
       sbuff=new StringBuffer();
       if (tok1.hasMoreTokens())
           sbuff.append("Class frequencies:\n");
       c=1;
       while (tok1.hasMoreTokens())
       {
           str=tok1.nextToken();
           sbuff.append("\tclass-"+(c++)+" : "+str+"\n");
           try  { freqs[c-2]=Integer.parseInt(str); }
           catch (NumberFormatException e)  { freqs[c-2]=0; }
       }
       Vcfreq.setText(Vcfreq.getText()+sbuff.toString());

     /*** class correlations ***/
       tok1=new StringTokenizer((String)prf.clscorrl.get("corrls"),"/");
       sbuff=new StringBuffer();
       if (tok1.hasMoreTokens())
           sbuff.append("Class correlations:");
       c=1;  i=0;
       while ((tok1.hasMoreTokens())&&(c<=48))
       {
           sbuff.append("\n\tclass-"+(c++)+": ");
           for ( i=0; i<48; i++ )
           {
               if (tok1.hasMoreTokens())
                   sbuff.append(tok1.nextToken());
               else
                   sbuff.append(".");

               sbuff.append(" ");
           }
       }
       Vccorrl.setText(sbuff.toString());

     /*** class notes (rejections) ***/
       Vcnotes.setText(((String)prf.clsnotes.get("rejections")).replace('$','\n'));
       try  { lovalue=Integer.parseInt((String)prf.clsnotes.get("lovalue")); }
       catch (NumberFormatException e)  { lovalue=0; }
       try  { hivalue=Integer.parseInt((String)prf.clsnotes.get("hivalue")); }
       catch (NumberFormatException e)  { hivalue=0; }
   }


   public void updateProgress( int value )
   {
    /*... text progress bar ...*/
       int   i, lim;
       StringBuffer sbuff;

       
       if ((value>=0)&&(value<=100))
       {
           sbuff=new StringBuffer();
           lim=(Vprog.getColumns()*value)/100;
           for ( i=0; i<lim; i++ )  sbuff.append("#");
           Lprog.setText("Progress:  "+value+"% completed");
           Vprog.setText(sbuff.toString());
       }
       else
       {
           Lprog.setText("Progress:");
           Vprog.setText("");
       }
    /*..........*/

    /*... graphic progress bar ...
        if (value<0)
            {  Vbar.updateValue(0);  Vbar.resetBar();  }
        else if (value>100)
            {  Vbar.updateValue(100);  Vbar.resetBar();  }
        else  // normal update
            Vbar.updateValue(value);
    ..........*/
   }

   public void winAnalysisState( boolean usewindows )
   {
       if (usewindows)
       {
           // System.out.println("enable windows");
           Vwin1.enable(); Vhi1.enable(); Vlo1.enable();
       }
       else  // disable window analysis
       {
           Vwin1.setText((Vseq1.getText().length()+1)+"");
           Vwin1.disable(); Vhi1.disable(); Vlo1.disable();
       }
   }


   public boolean action( Event evt, Object what )
   {
       if (evt.target.equals(btnSend))
       {
           if (checkFormData())
           {
               if (makeRequest())
               {
                   if (waitforans)  switchToCard3("");
                   else  switchToCard1("Ready for new request");
               }
               else
               {
                   btnRetry.enable();
                   switchToCard2("Failed: Request not completed");
               }
           }
       }
       else if (evt.target.equals(btnReset))
       {
           Vseq1.setText("");
           Vuser1.setText("");
           Vmail1.setText("");
           //Vname1.setText("");
           
           Vproc1.select(0);
           Vwin1.setText("0");
           // winAnalysisState(true);
       }
       else if (evt.target.equals(btnCancel))
       {
           // OKDialog.exec("Information","Request has been cancelled.");           
           getAppletContext().showStatus("Information: Request has been cancelled");
           switchToCard1("Request has been cancelled");
       }
       else if (evt.target.equals(btnClose))
       {
           // OKDialog.exec("Information","Prediction completed.");
           getAppletContext().showStatus("Information: Prediction completed");
           switchToCard1("Ready for new request");
       }
       else if (evt.target.equals(btnRetry))
       {
           if (makeRequest())
           {
               if (waitforans)  switchToCard3("");
               else  switchToCard1("Ready for new request");
           }
           else
               switchToCard2("Failed: Request not completed");
       }
       else if (evt.target.equals(Vusewin))
       {
           winAnalysisState(Vusewin.getState());
       }
           
       return super.action(evt,what);
   }
     
}


        
     
