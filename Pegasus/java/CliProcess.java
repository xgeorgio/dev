import java.lang.*;
import java.net.*;
import java.io.*;
import java.util.*;



public class CliProcess extends Object implements Runnable
{
    protected SvrMain   svr;
    protected Socket    cliSock;
    protected int       cliID, cliPrio;
    protected CliRegistry   cliReg;
    protected PredProfile   reqProf;
    public Thread    cliProc;
    //protected DataInputStream cliRecv;
    //protected PrintStream cliSend;

    

    public CliProcess( SvrMain parent, Socket client, int cliNum, CliRegistry registry, int prio )
    {
        svr=parent;
        cliSock=client;
        cliID=cliNum;
        cliReg=registry;
        cliPrio=prio;

        cliProc=new Thread(this,""+cliID);
        cliProc.setPriority(cliPrio);
        cliProc.start();
    }


    public boolean equals( Object anotherObject )
    {
        CliProcess  aobj=(CliProcess)anotherObject;
        boolean  res;

        res=cliReg.equals(aobj.cliReg)&&(cliID==aobj.cliID);
        res=res&&cliProc.equals(aobj.cliProc);

        return(res);
    }


    public int hashCode()
    {
        long    code;

        code=cliReg.hashCode()+cliID+cliProc.hashCode();
        return((int)(code%Integer.MAX_VALUE));
    }


    public String toString()
    {
        StringBuffer sbuff=new StringBuffer();

        sbuff.append("[ client process: "+cliID+"\n");
        sbuff.append(cliReg.toString());
        if (cliProc!=null)  sbuff.append("\nstate: running  ]");
        else  sbuff.append("\nstate: idle  ]");
        
        return(sbuff.toString());
    }
        
        
    public void run()
    {
        takeRequest();
        stopProcess();
    }


    public void stopProcess()
    {
        cliProc.stop();
        cliProc=null;
    }

    
    public boolean takeRequest()
    {
        boolean     result=false, foundincache;
        String      cliRequest, usermail, clientag;
        String      fproc, fmail, fsock, fserv, flev;


        svr.serverMessage("Client #"+cliID+": started (TID="+cliProc.getName()+")");
        
      /*** receive data profile (request) ***/
        svr.serverMessage("Client #"+cliID+": receiving request data");
        reqProf=new PredProfile();
        if (reqProf.recvProfile(cliSock))
        {
            // System.out.println("received request:\n"+reqProf.toString()+"\n");

            reqProf.header.put("server",svr.versionTag);
            reqProf.header.put("usertype","level-"+cliPrio);

          /*** check cache first ***/
            foundincache=false;
            if (svr.usecache)
            {
                if (svr.cache.containsProfile(reqProf))
                {
                    try
                    {
                      /* save original request tag */
                        usermail=new String((String)reqProf.header.get("email"));
                        clientag=new String((String)reqProf.header.get("client"));
                        fserv=new String((String)reqProf.header.get("server"));
                        flev=new String((String)reqProf.header.get("usertype"));
                        fproc=new String((String)reqProf.header.get("proc"));
                        fmail=new String((String)reqProf.header.get("sendmail"));
                        fsock=new String((String)reqProf.header.get("sendsock"));

                      /* retrieve request data */
                        reqProf=svr.cache.getFullProfile(reqProf);
                        foundincache=true;

                      /* restore original request tag */
                        reqProf.header.put("server",fserv);
                        reqProf.header.put("email",usermail);
                        reqProf.header.put("client",clientag);
                        reqProf.header.put("proc",fproc);
                        reqProf.header.put("usertype",flev);                        
                        reqProf.header.put("sendmail",fmail);
                        reqProf.header.put("sendsock",fsock);
                        reqProf.header.put("outcome","success");

                        boolean  mailsent=true, socksent=true;
        
                      /* send mail */
                        if (((String)reqProf.header.get("sendmail")).equals("true"))
                        {
                            svr.serverMessage("Client #"+cliID+": mailing result data");
                            mailsent=mailResults(reqProf);
                            if (mailsent)
                                reqProf.header.put("sendmail","true");
                            else
                                reqProf.header.put("sendmail","false");
                        }
        
                      /* send request */
                        if (((String)reqProf.header.get("sendsock")).equals("true"))
                        {
                            svr.serverMessage("Client #"+cliID+": sending result data");
                            socksent=reqProf.sendProfile(cliSock);
                            // cliSend.println(reqProf.toTransString());
                            
                            if (socksent)
                                reqProf.header.put("sendsock","true");
                            else
                                reqProf.header.put("sendsock","false");
                        }

                        result = mailsent && socksent;
                    }
                    catch (Exception e)
                      { result=false; }
                }
            }

            if (!foundincache)
            {
                result=spoolRequest(reqProf);
                if (result)  svr.cache.put(reqProf);
            }

            if (result)
                svr.serverMessage("Client #"+cliID+": completed");
            else
                svr.serverMessage("Client #"+cliID+": failed");
        }
        else
        {
            svr.serverMessage("Client #"+cliID+": rejected (recv.conn.error)");
            /*try
              { cliRecv.close();  cliSend.close(); }
            catch (Exception e21)  {}
            finally
              { return(false); }*/
            result=false;
        }
        
      /*** close streams (socket is closed by server main) ***/
        /*try
          { cliRecv.close(); cliSend.close(); }
        catch (Exception e90)  {}*/
        
        return(result);
    }


    public boolean spoolRequest( PredProfile requestProfile )
    {
        DataInputStream     resultFile;
        FileInputStream     pred1File, pred2File, pred3File;
        PrintStream         requestFile;
        String          reqString, resString, wreqString;
        String          stemp=new String(), stoken=new String();
        StringBuffer    sbuff=new StringBuffer();
        StringTokenizer     str2tok;
        float           fval=(float)0.0;
        Integer         ival;
        int         curClass, class1=0, class2=0, class3=0, majClass;
        int         wsize=0, i, c, wcount=0, prev, curr;
        int         cfreqcount[]=new int[48], ccorrls[][]=new int[48][48];
        boolean         readstate, valid;
        Vector          winseqs, selections, frequencies;
        Enumeration     enum;

     

      /*** Initialize result fields ***/

        reqProf.totalp.put("prediction","");
        reqProf.windowp.put("results","");
        reqProf.windowp.put("winseqs","");
        reqProf.windowp.put("prediction","");
        reqProf.clsfreq.put("freqs","");
        reqProf.clsfreq.put("selections","");
        reqProf.clscorrl.put("corrls","");
        reqProf.clsnotes.put("rejections","");
        reqProf.clsnotes.put("lovalue","");
        reqProf.clsnotes.put("hivalue","");

        reqString=(String)requestProfile.inpseq.get("sequence");


      /***** Execute TOTAL prediction *****/
      
        valid=svr.predictor.calculate(reqString);

        if (!valid)
        {
            svr.serverMessage("Client #"+cliID+": rejected (data.format.error)");
            resString=new String("Server: ERROR: Unable to execute valid prediction.");
            requestProfile.header.put("outcome",resString);
            requestProfile.sendProfile(cliSock);
            
            return(false);
        }

      /*... Choose majority prediction ...*/

        class1=svr.predictor.result[0]+1;
        class2=svr.predictor.result[1]+1;
        class3=svr.predictor.result[2]+1;       
        majClass=chooseMajority(class1,class2,class3);

        sbuff.append(majClass);
        requestProfile.totalp.put("prediction",sbuff.toString());
            

      /***** Execute WINDOW prediction(s) *****/
      
      /*... Initializing analysis parameters ...*/

        try
        {
            wsize=Integer.parseInt((String)reqProf.params.get("winsize"));
            valid=(wsize>0);
        }
        catch (Exception e)
          { valid=false; }

        if (!valid)
        {
            svr.serverMessage("Client #"+cliID+": rejected (win.size.error)");
            resString=new String("Server: ERROR: Invalid window size");
            requestProfile.header.put("outcome",resString);
            requestProfile.sendProfile(cliSock);           
            return(false);
        }
            
        winseqs=new Vector();
        selections=new Vector();
        frequencies=new Vector();
        sbuff=new StringBuffer();

        //for ( i=0; i<48; i++ )
        //    cfreqcount[i]=0;
        
        for ( i=0; i<48; i++ )
        {
            cfreqcount[i]=0;
            for ( c=0; c<48; c++ )  ccorrls[i][c]=0;
        }

        for ( i=0; (i+wsize-1)<=(reqString.length()-1); i++ )
        {
            wreqString=new String(reqString.substring(i,i+wsize));
            winseqs.addElement(wreqString);
        }


      /*... Executing per-window analysis ...*/

        enum=winseqs.elements();
        while (enum.hasMoreElements())
        {
            wreqString=(String)enum.nextElement();

            //System.gc();

            wcount++;

            valid=svr.predictor.calculate(wreqString);

            if (!valid)
            {
                svr.serverMessage("Client #"+cliID+": rejected (data.format.error)");
                resString=new String("Server: ERROR: Unable to execute valid prediction.");
                requestProfile.header.put("outcome",resString);
                requestProfile.sendProfile(cliSock);
            
                return(false);
            }

          /*... Choose majority prediction ...*/

            class1=svr.predictor.result[0]+1;
            class2=svr.predictor.result[1]+1;
            class3=svr.predictor.result[2]+1;       
            majClass=chooseMajority(class1,class2,class3);

            selections.addElement(new Integer(majClass));

            if ((majClass>0)&&(majClass<=48))   //---> do not update for -1 (bad prediction)
                cfreqcount[majClass-1]++;

            sbuff.append(majClass);
            if (enum.hasMoreElements())  sbuff.append("/");
        }
      

      /*... Create final analysis results ...*/

       /* window predictions */
        requestProfile.windowp.put("results",sbuff.toString());

        enum=winseqs.elements();
        sbuff=new StringBuffer();
        while (enum.hasMoreElements())
        {
            sbuff.append((String)enum.nextElement());
            if (enum.hasMoreElements())  sbuff.append("/");
        }
        requestProfile.windowp.put("winseqs",sbuff.toString());

       /* frequencies */
        if (wcount>0)
        {
            int maxcount=0;
            int lobound=0, hibound=0, lovalue, hivalue;

            for ( i=0; i<48; i++ )
            {
                frequencies.addElement(new Integer(cfreqcount[i]));
                if (cfreqcount[i]>cfreqcount[maxcount])
                    maxcount=i;
            }

            enum=frequencies.elements();
            sbuff=new StringBuffer();
            while (enum.hasMoreElements())
            {
                sbuff.append(((Integer)enum.nextElement()).toString());
                if (enum.hasMoreElements())  sbuff.append("/");
            }
            requestProfile.clsfreq.put("freqs",sbuff.toString());

            enum=selections.elements();
            sbuff=new StringBuffer();
            while (enum.hasMoreElements())
            {
                sbuff.append(((Integer)enum.nextElement()).toString());
                if (enum.hasMoreElements())  sbuff.append("/");
            }
            requestProfile.clsfreq.put("selections",sbuff.toString());

          /*** make window prediction, based on higher class frequency ***/
            requestProfile.windowp.put("prediction",(new Integer(maxcount+1)).toString());

       /* notes (rejections) */
            sbuff=new StringBuffer();
            int     iival;
            Float   ffval;

            try
            {
                iival=Integer.parseInt((String)requestProfile.params.get("lorejlim"));
                ffval=new Float((new Integer(iival)).toString());
                ffval=new Float((((float)cfreqcount[maxcount])*ffval.floatValue())/100.0);
                lovalue=Math.round(ffval.floatValue());
            }
            catch (NumberFormatException e)
              { lovalue=0; }

            try
            {
                iival=Integer.parseInt((String)requestProfile.params.get("hirejlim"));
                ffval=new Float((new Integer(iival)).toString());
                ffval=new Float((((float)cfreqcount[maxcount])*ffval.floatValue())/100.0);
                hivalue=Math.round(ffval.floatValue());
            }
            catch (NumberFormatException e)
              { hivalue=0; }

            sbuff.append("Max class freq. selection:    class-"+(maxcount+1)+" (freq="+cfreqcount[maxcount]+")$");
            sbuff.append("Total (low) rejection limit:  freq<"+lovalue+"$");
            sbuff.append("Near (high) rejection limit:  freq>"+hivalue+"$");
            sbuff.append("Rejection summary:$");
            
            for ( i=0; i<48; i++ )
            {
                if (cfreqcount[i]<lovalue)
                    sbuff.append("Total (low) rejection:  class-"+(i+1)+" (freq="+cfreqcount[i]+")$");

                if ((cfreqcount[i]>hivalue)&&(i!=maxcount))
                    sbuff.append("Near (high) rejection:  class-"+(i+1)+" (freq="+cfreqcount[i]+")$");
            }

            requestProfile.clsnotes.put("rejections",sbuff.toString());
            requestProfile.clsnotes.put("lovalue",(new Integer(lovalue)).toString());
            requestProfile.clsnotes.put("hivalue",(new Integer(hivalue)).toString());
        }

       /* correlations */
        if (wcount>0)
        {
            prev=-1;
            enum=selections.elements();
            while (enum.hasMoreElements())
            {
                curr=((Integer)enum.nextElement()).intValue();
                if ((prev>0)&&(curr>0))    //---> can be -1 after bad prediction
                {
                    ccorrls[curr-1][prev-1]++;
                    ccorrls[prev-1][curr-1]=ccorrls[curr-1][prev-1];
                }
                prev=curr;
            }
            
            sbuff=new StringBuffer();
            for ( i=0; i<48; i++ )
            {
                for ( c=0; c<48; c++ )
                {
                    sbuff.append(ccorrls[i][c]);
                    if (c!=47)  sbuff.append("/");
                }
                if (i!=47)  sbuff.append("/");
            }

            requestProfile.clscorrl.put("corrls",sbuff.toString());
        }
                    

      /***** Send all results *****/

        requestProfile.header.put("outcome","success");
        boolean  mailsent=true, socksent=true;
        
        if (((String)requestProfile.header.get("sendmail")).equals("true"))
        {
            svr.serverMessage("Client #"+cliID+": mailing result data");
            mailsent=mailResults(requestProfile);
            if (mailsent)
                requestProfile.header.put("sendmail","true");
            else
                requestProfile.header.put("sendmail","false");
        }
        
        //System.out.println("sending profile: \n"+requestProfile.toString());
        if (((String)requestProfile.header.get("sendsock")).equals("true"))
        {
            svr.serverMessage("Client #"+cliID+": sending result data");

            if (cliSock!=null)
                socksent=requestProfile.sendProfile(cliSock);
            else        
            {
                svr.serverMessage("Client #"+cliID+": connection broken (send.sock.error)");
                socksent=false;
            }

            if (socksent)
                requestProfile.header.put("sendsock","true");
            else
                requestProfile.header.put("sendsock","false");
        }

        return(mailsent&&socksent);
    }
    

    public boolean mailResults( PredProfile prf )
    {
        String          email, context[];
        int             currline=0;
        StringTokenizer stoken;

        stoken=new StringTokenizer(prf.toTransString(),"&");

        context=new String[stoken.countTokens()];

        while ((stoken.hasMoreTokens())&&(currline<context.length))
        {
            context[currline]=new String(stoken.nextToken());
            currline++;
        }

        email=(String)prf.header.get("email");

        return(svr.mailer.sendMail(email,context));
    }

    
    public int chooseMajority( int a1, int a2, int a3 )
    {
        if ((a1==a2)&&(a1==a3))
            return(a1);
        else if (a1==a2)
            return(a1);
        else if (a1==a3)
            return(a1);
        else if (a2==a3)
            return(a2);
        else
            return(-1);
    }

}



