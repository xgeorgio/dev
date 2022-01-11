import java.lang.*;
import java.io.*;
import java.util.*;



public class UserDatabase extends Object
{
    protected boolean consistent;
    protected Hashtable  database;
    public static String userDBName="users.DB";
    protected static String DBTag="Pegasus User database file";


    
    public UserDatabase()
    {
        super();
        database=new Hashtable();
        consistent=false;
    }


    public boolean isConsistent()
    {
        return(consistent);
    }


    public boolean containsUser( String userMail, String userID )
    {
        String  userTag=new String(userMail+":"+userID);

        return(database.containsKey(userTag));
    }

    
    public int queryUser( String userMail, String userID ) throws IllegalAccessException
    {
        if (containsUser(userMail,userID))
        {
            return( ((Integer)database.get(userMail+":"+userID)).intValue() );
        }
        else
        {
          /*** no default priority, just raise an error ***/
            throw (new IllegalAccessException("not found"));
        }
    }


    public void removeUser( String userMail, String userID )
    {
        if (containsUser(userMail,userID))
        {
            String userTag=new String(userMail+":"+userID);            
            database.remove(userTag);
            consistent=false;
        }
    }
    
        
    public void addUser( String userMail, String userID, int setPrio )
    {
        Integer     clsPrio;
        String      userTag;

        
        if (containsUser(userMail,userID))
            removeUser(userMail,userID);

        clsPrio=new Integer(setPrio);
        userTag=new String(userMail+":"+userID);
        database.put(userTag,clsPrio);
        consistent=false;   // already set by removeUser
    }


    public boolean readDatabase()
    {
        File            DBFile;
        BufferedReader  DBStream;
        String      userRec, userMail, userID;
        int         clsPrio;
        boolean     eof, res=false;



        database.clear();
        consistent=false;

        //--- clear any fileIDs left allocated ---
        System.gc();

        DBFile=new File(userDBName);
        if (DBFile.isFile()&&DBFile.canRead())
        {
            try
            {
                DBStream=new BufferedReader(new FileReader(userDBName),1000);

              /*** read two lines of comments ***/
                DBStream.readLine();
                DBStream.readLine();

                eof=false;
                while ((!eof) && DBStream.ready())
                {
                    try
                    {
                        userRec=DBStream.readLine();
                        userMail=new String(userRec.substring(0,userRec.indexOf(":")));
                        userID=new String(userRec.substring(userRec.indexOf(":")+1,userRec.lastIndexOf(":")));
                        clsPrio=Integer.parseInt(userRec.substring(userRec.lastIndexOf(":")+1));

                        // System.out.println(userMail+"_"+userID+"_"+clsPrio);
                        addUser(userMail,userID,clsPrio);
                        res=true;
                    }
                    catch (NumberFormatException e0)
                    {
                        System.out.println("error: invalid user database file record ("+userDBName+")");
                        consistent=false;
                        res=false;
                        eof=true;  //break;
                    }
                    //catch (IOException e1)
                      //{ eof=true;  //break; }
                }

                DBStream.close();
            }
            catch (FileNotFoundException e2)
            {
                System.out.println("error: user database file not found ("+userDBName+")");
                res=false;  
            }
            catch (IOException e3)
            {
                System.out.println("error: unable to read from user database file ("+userDBName+")");
                res=false;
            }
        }
        else   // access file: error
        {
            System.out.println("error: unable to open user database file ("+userDBName+")");
            res=false;
        }

        DBFile=null;
        if (res)  consistent=true;
        return(res);
    }
    

    public boolean writeDatabase()
    {
        File            DBFile;
        PrintWriter     DBStream;
        String      userTag;
        Integer     clsPrio;
        boolean     res=false;



        consistent=false;

        //--- clear any fileIDs left allocated ---
        System.gc();

        try
        {
            DBStream=new PrintWriter(new FileWriter(userDBName),true);
            Enumeration enum;

          /*** write two lines of comments ***/
            Date d=new Date();
            DBStream.println("#"+DBTag);
            DBStream.println("#"+d.toString());

            enum=database.keys();
            while (enum.hasMoreElements())
            {
                userTag=(String)enum.nextElement();
                clsPrio=(Integer)database.get(userTag);
                DBStream.println(userTag+":"+clsPrio.toString());

                if (DBStream.checkError())
                  { break; }
            }

            DBStream.flush();

            if (DBStream.checkError())
            {
                System.out.println("error: unable to write to user database file ("+userDBName+")");
                res=false;
            }
            else  res=true;

            DBStream.close();
        }
        catch(FileNotFoundException e)
        {
            System.out.println("error: user database file not found ("+userDBName+")");
            res=false;
        }
        catch (IOException e)
        {
            System.out.println("error: unable to write to user database file ("+userDBName+")");
            res=false;
        }

        DBFile=null;
        if (res)  consistent=true;
        return(res);
    }
    

    public boolean updateDatabase()
    {
        if (!consistent)  consistent=writeDatabase();
        // else  consistent=true;

        return(consistent);
    }    


    public void removeAll()
    {
        database.clear();
        consistent=false;
    }

    
    public String toString()
    {
        Enumeration enum;
        StringBuffer sbuff=new StringBuffer();
        String      userTag;
        

        sbuff.append("userDB:\n[\n");

        enum=database.keys();
        while(enum.hasMoreElements())
        {
            userTag=(String)enum.nextElement();
            sbuff.append(userTag+":"+((Integer)database.get(userTag))+"\n");
        }
        sbuff.append("]");

        return(sbuff.toString());
    }



/************/
    public static void main( String args[] )
    {
        UserDatabase db=new UserDatabase();
        
        System.out.println("\nREADING...");
        db.readDatabase();
        
        System.out.println("\nADDING...");
        db.addUser("xgeorgio","",Thread.MIN_PRIORITY);
        db.addUser("xgeorgio","555",Thread.NORM_PRIORITY);
        db.addUser("xgeorgio.aurora.eexi.gr","admin0",Thread.NORM_PRIORITY);

        System.out.println("\nConsistency ?  "+db.isConsistent());
        
        System.out.println("\nSEARCHING...");
        System.out.println("\txgeorgio:555 ?  "+db.containsUser("xgeorgio","555"));

        System.out.println("\nUPDATING...");
        db.updateDatabase();
        
        System.out.println("\n"+db.toString());
    }
/**************/

}


