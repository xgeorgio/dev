import java.lang.*;
import java.io.*;
import java.util.*;



public class ProfileCache extends ObjectList
{
    protected String cacheDirName;
    protected boolean consistent;
    public static String defCacheDirName="cache";
    public static String cacheFilePrefix="_c";
    public static String cacheFileSuffix=".prof";


    
    public ProfileCache()
    {
        super();
        consistent=false;
    }


    public ProfileCache( int initialCapacity )
    {
        super(initialCapacity);
        consistent=false;
    }


    public ProfileCache( int initialCapacity, int capacityIncrement )
    {
        super(initialCapacity,capacityIncrement);
        consistent=false;
    }


    public String getDirName()
    {
        return(cacheDirName);
    }


    public boolean initCacheDir()
    {
        return(initCacheDir(defCacheDirName));
    }


    public boolean isConsistent()
    {
        return(consistent);
    }


    public int lookupProfile( PredProfile queryProf )
    {
        Enumeration  enum;
        PredProfile  curProf;
        boolean  found;
        int     pos=-1;


        if (queryProf!=null)
        {
            enum=elements();
            while (enum.hasMoreElements())
            {
                try
                {
                    curProf=((PredProfile)enum.nextElement());
                    found=( ((String)curProf.inpseq.get("sequence")).equals((String)queryProf.inpseq.get("sequence")) );
                    if (found)
                        found=found&&( ((String)curProf.params.get("winsize")).equals((String)queryProf.params.get("winsize")) );
                    if (found)
                        found=found&&( ((String)curProf.params.get("lorejlim")).equals((String)queryProf.params.get("lorejlim")) );
                    if (found)
                        found=found&&( ((String)curProf.params.get("hirejlim")).equals((String)queryProf.params.get("hirejlim")) );

                    if (found)  pos=indexOf(curProf);
                }
                catch (Exception e)
                  { pos=-1; }
            }
        }
        // else
        //     pos=-1;

        return(pos);
    }


    public boolean containsProfile( PredProfile queryProf )
    {
        return(lookupProfile(queryProf)>=0);
    }

    
    public PredProfile getFullProfile( PredProfile queryProf ) throws IllegalAccessException
    {
        PredProfile prf=new PredProfile();
        int pos=lookupProfile(queryProf);

        if (pos>=0)
            prf.mirror((PredProfile)elementAt(pos));
        else
            throw (new IllegalAccessException("not found"));

        return(prf);    // only when profile was found
    }
    

    public void put( Object prf )
    {
        if ( ((contains(prf))&&(isOnHold())) || ((!contains(prf))&&(!isOnHold())) )
            consistent=false;
        else
            consistent=true;

        super.put(prf);
    }


    public void touch( Object prf )
    {
        if ( (contains(prf))&&(isOnHold()) )
            consistent=false;
        else
            consistent=true;

        super.touch(prf);
    }

    
    public boolean initCacheDir( String dirName )
    {
        File    cacheDir;
        boolean res;
        
        
        if (dirName.equals(""))
            cacheDirName=new String(defCacheDirName);
        else
            cacheDirName=new String(dirName);

        cacheDir=new File(cacheDirName);
        if (cacheDir.isDirectory()&&cacheDir.canWrite())
            res=clearCacheDir();
        else
            res=cacheDir.mkdir();

        if ((res)&&(occupied()==0))  consistent=true;
        else  consistent=false;

        cacheDir=null;
        System.gc();
        return(res);
    }
    
        
    public boolean clearCacheDir()
    {
        File    delFile, cacheDir;
        boolean     noerr=true, res;
        int     i=1;


        System.gc();
        cacheDir=new File(cacheDirName);
        if (cacheDir.isDirectory()&&cacheDir.canWrite())
        {
            noerr=true;
            while (cacheDir.list().length>0)
            {
                delFile=new File(cacheDir,cacheDir.list()[0]);
                noerr=delFile.isFile()&&delFile.canWrite();
                if (noerr)  noerr=delFile.delete();
                delFile=null;
            }

            res=noerr;
        }
        else
        {
            System.out.println("clear: invalid directory");
            res=false;
        }

        if ((res)&&(occupied()==0))  consistent=true;
        else  consistent=false;

        cacheDir=null;
        return(res);
    }


    public int readCache()
    {
        PredProfile curProf;
        File        recFile, cacheDir;
        String      fullFileName;
        int         i, recRead, currsz;
        boolean     noerr;


        removeAllElements();
        consistent=false;

        noerr=true;
        System.gc();

        cacheDir=new File(cacheDirName);
        if (cacheDir.isDirectory()&&cacheDir.canRead())
        {
            for ( i=0,recRead=0; (noerr)&&(i<cacheDir.list().length); i++ )
            {
                recFile=new File(cacheDir,cacheDir.list()[i]);
                noerr=recFile.isFile()&&recFile.canRead();
                if (noerr)
                {
                    curProf=new PredProfile();
                    fullFileName=new String(cacheDir.getAbsolutePath()+File.separator+recFile.getName());
                    noerr=curProf.readProfile(fullFileName);
                    if (noerr)
                    {
                        put(curProf);
                        recRead++;
                        recFile=null;
                    }
                }
            }

            consistent = noerr;
        }
        else   // access directory: error
        {
            recRead=-1;
            consistent=false;
        }

        cacheDir=null;
        return(recRead);
    }
    

    public boolean writeCache( boolean clearfirst )
    {
        File        cacheDir;
        String      recFileName;
        String      fullFileName;
        Enumeration     enum;
        PredProfile     curProf;
        String          curSeq;
        boolean     noerr, res;
        int         currsz;



        currsz=limit();
        hold();         // make sure no new objects are added

        if (clearfirst)  noerr=clearCacheDir();
        else  noerr=true;
        
        consistent = consistent && noerr;

        if (noerr)
        {
            System.gc();
            
            cacheDir=new File(cacheDirName);
            if (cacheDir.isDirectory()&&cacheDir.canWrite())
            {
                enum=elements();
       
                while ((noerr)&&(enum.hasMoreElements()))
                {
                    curProf=(PredProfile)enum.nextElement();
                    if (curProf.inpseq.containsKey("sequence"))
                    {
                        curSeq=(String)curProf.inpseq.get("sequence");
                        recFileName=new String(cacheFilePrefix+curSeq.hashCode()+cacheFileSuffix);
                        fullFileName=new String(cacheDir.getAbsolutePath()+File.separator+recFileName);
                        // System.out.println("Writing file: "+fullFileName);
                        noerr=curProf.writeProfile(fullFileName);
                    }
                    else
                        System.out.println("error: profile: invalid data, not written");
                }

                consistent = consistent && noerr;
            }
            else   // access directory: error
            {
                res=false;
                consistent=false;
            }
        }
        else   // clearCacheDir: error
        {
            res=false;
            consistent=false;
        }

        setLimit(currsz);
        cacheDir=null;
        return(noerr);
    }
        

    public boolean writeCache()
    {
        return(writeCache(true));
    }


    public boolean updateCacheDir()
    {
        if (!consistent)  consistent=writeCache();
        // else  consistent=true;

        return(consistent);
    }
    

    public String ToString()
    {
        StringBuffer sbuff=new StringBuffer();
        sbuff.append("cache:\n[\n");
        sbuff.append(super.ToString()+"]\n");

        return(sbuff.toString());
    }


/************
    public static void main( String args[] )
    {
        ProfileCache c=new ProfileCache();
        PredProfile p1, p2;
        

        System.out.println("INITIALIZING...");

        p1=new PredProfile();
        p1.inpseq.put("sequence","TTTTAAAA");
        p1.params.put("winsize","20");
        p1.params.put("lorejlim","10");
        p1.params.put("hirejlim","5");
        p1.header.put("client","dummy client tag");
        c.put(p1);

        p2=new PredProfile();
        p2.inpseq.put("sequence","TTTTAAAA");
        p2.params.put("winsize","20");
        p2.params.put("lorejlim","10");
        p2.params.put("hirejlim","5");
        
        
        System.out.println("\nSEARCHING...");
        if (c.containsProfile(p2))
        {
            System.out.println("profile: found");
            try
            {
                System.out.println("position="+c.lookupProfile(p2));
                System.out.println("contents=\n"+((PredProfile)c.getFullProfile(p2)).toString());
            }
            catch (IllegalAccessException e)
              { System.out.println("error: unable to locate profile"); }
        }
        else
            System.out.println("profile: NOT found");
    }
**************/

}


