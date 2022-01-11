import java.lang.*;


public class ClientQueue extends ObjectPQueue
{
    public ClientQueue()
    {
        super();
    }
    

/************
    public static void main( String args[] )
    {
        ObjectPQueue q=new ObjectPQueue();
    
        q.push(new CliRegistry("TTAGAGGAT.1","xgeorgio",8881),  Thread.NORM_PRIORITY);
        q.push(new CliRegistry("TTAGGAT.2","xgeorgio",8881),  Thread.MAX_PRIORITY);
        q.push(new CliRegistry("TTAGAG.3","xgeorgio",8881),  Thread.MIN_PRIORITY);
        q.push(new CliRegistry("TTAGAGGATAAA.4","xgeorgio",8881),  Thread.NORM_PRIORITY);

        System.out.println(q.toString());

        try
        {
            System.out.println("Pop sequence:");
            while (!q.isEmpty())
                System.out.print(((CliRegistry)q.pop()).toString()+"  ");
            System.out.println();
        }
        catch (Exception e)
          { e.printStackTrace(); }
    }
*************/    

}


