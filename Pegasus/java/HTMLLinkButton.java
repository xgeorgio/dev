import java.applet.*;
import java.awt.*;
import java.net.*;


public class HTMLLinkButton extends Applet
{
    String htmlLink;
    URL URLLink;
    String caption;
    Button btnGoto;
    
    
    public void init()
    {
        // System.out.println("CodeBase="+getCodeBase().toString());
        // System.out.println("LinkPage="+getParameter("LINK"));
        // System.out.println("Caption="+getParameter("CAPTION"));

        htmlLink=new String(getCodeBase().toString()+getParameter("LINK"));
        caption=new String(getParameter("CAPTION"));
        btnGoto=new Button(caption);
     
        try
            { URLLink=new URL(htmlLink); }
        catch (MalformedURLException e)
            { try {URLLink=new URL(getCodeBase().toString());}
              catch (MalformedURLException e0)  {}  }
        finally
        {
            setLayout(new BorderLayout());
            add("Center",btnGoto);
            resize(btnGoto.preferredSize().width+5,btnGoto.preferredSize().height+5);
            show();
        }
    }
    
    public boolean action( Event evt, Object what )
    {
        if (evt.target.equals(btnGoto))
        {
            // System.out.println(URLLink.toString());
            getAppletContext().showDocument(URLLink,"_self");
            return(true);
        }
        
        return(super.action(evt,what));
    }
}


