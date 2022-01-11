import java.lang.*;
import java.util.*;
import java.io.*;



public class SvrConfig
{
    public Properties    params;
    public String        cfgFileName;
    public String        cfgHeader;
    protected BufferedInputStream    cfgInFile;
    protected BufferedOutputStream   cfgOutFile;
    


    public SvrConfig( String cfgfname, String cfghead )
    {
        Properties defs;


      /*** Create default parameters values ***/
      /*-- Note: application-specific code --*/
        defs=new Properties();

        defs.put(new String("port"),new String("8881"));
        defs.put(new String("tty"),new String("true"));
        defs.put(new String("log"),new String("false"));
        defs.put(new String("logfile"),new String("SvrMain.log"));
        defs.put(new String("cache"),new String("true"));
        defs.put(new String("cachesize"),new String("10"));
        defs.put(new String("database"),new String("true"));
        defs.put(new String("queue"),new String("true"));


      /*** Initialize ***/
        params=new Properties(defs);
        cfgFileName=new String(cfgfname);
        cfgHeader=new String(cfghead);
    }


    public boolean readParamFile()
    {
        boolean     res;
        

        try
        {
            cfgInFile=new BufferedInputStream(new FileInputStream(cfgFileName),1000);
            params.load(cfgInFile);
            res=true;
        }
        catch (FileNotFoundException e10)
        {
            System.out.println("Error: configuration file not found ("+cfgFileName+")");
            res=false;
        }
        catch (IOException e11)
        {
            System.out.println("Error: unable to read configuration file ("+cfgFileName+")");
            res=false;
        }

        if (cfgInFile!=null)
          {  try  { cfgInFile.close(); }  catch (IOException e13) {}  }

        return(res);
    }

    
    public int writeParamFile()
    {
        int     res;

        
        try
        {
            File        ftemp=new File(cfgFileName);

            if (!ftemp.delete())
            {
                // System.out.println("Creating new (default) configuration file");
                cfgOutFile=new BufferedOutputStream(new FileOutputStream(cfgFileName),1000);
                PrintWriter cfgOut=new PrintWriter(cfgOutFile,true);
                Enumeration     enump=params.propertyNames();
                String      pname, pval;

                params.save(cfgOutFile,cfgHeader);
                while (enump.hasMoreElements())
                {
                    pname=(String)enump.nextElement();
                    pval=params.getProperty(pname);
                    cfgOut.println(pname+"="+pval);
                }

                cfgOut.close();
                
                res=1;
            }
            else
            {
                cfgOutFile=new BufferedOutputStream(new FileOutputStream(cfgFileName),1000);
                params.save(cfgOutFile,cfgHeader);

                res=0;
            }

            if (cfgOutFile!=null)   {  cfgOutFile.close();  }
        }
        catch (IOException e21)
        {
            System.out.println("Error: unable to write configuration file");
            res=-1;
        }

        return(res);
    }
    

/**************
    public static void main( String args[] )
    {
        SvrConfig cfg=new SvrConfig("SvrMain.cfg","Server configuration file");

        System.out.println("Configuration file ("+cfg.cfgFileName+"):\n");
        cfg.params.list(System.out);

        System.out.print("\nSave before exit: ");
        switch (cfg.writeParamFile())
        {
            case -1: System.out.println("(error)"); break;
            case  0: System.out.println("(keep)"); break;
            case  1: System.out.println("(create)"); break;
            default: System.out.println("(unknown)"); break;
        }
    }
***************/

}
    
        
