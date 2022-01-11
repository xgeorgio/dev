import java.util.*;



public class ProteinChars extends Vector
{
    public ProteinChars()
    {
      /*** Create array of valid protein symbols ***/
      // Note: letters not used={J,O,U}
      //       synonyms: {B=N,Z=Q} -- currently not in use!

        super(20);

        addElement(new Character('A'));
        addElement(new Character('C'));
        addElement(new Character('D'));
        addElement(new Character('E'));
        addElement(new Character('F'));
        addElement(new Character('G'));
        addElement(new Character('H'));
        addElement(new Character('I'));
        addElement(new Character('K'));
        addElement(new Character('L'));
        addElement(new Character('M'));
        addElement(new Character('N'));
        addElement(new Character('P'));
        addElement(new Character('Q'));
        addElement(new Character('R'));
        addElement(new Character('S'));
        addElement(new Character('T'));
        addElement(new Character('V'));
        addElement(new Character('W'));
        addElement(new Character('Y'));
    }


    public boolean isValid( String str )
    {
       for ( int i=0; i<str.length(); i++ )
       {
           if (!contains(new Character(str.charAt(i))))
               return(false);
       }

       return(true);
    }

}

