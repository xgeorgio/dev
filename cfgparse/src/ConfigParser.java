/**
 * Package: (gereal tools)
 * Class: ConfigParser
 * Description: Parse and retrieve structured key-value pairs. This is a lightweight
 *              string-based parser of key-value pairs with or without section labels.
				Section labels should be enclosed on brackets '[' and ']', multi-value
				keys should begin with asterisk '*' to flag multiple token parsing,
				comments are marked with ';' and text is skipped therafter.
				See '/examples' folder and corresponding output for capabilities.
 * Updated: 5-Feb-2022 22:09'
 * <p>
 * @author Harris Georgiou (c) 2022
 * @version 1.1
 * @see -
 */

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.Map;
import java.util.HashMap;
import java.util.StringTokenizer;
 

/**
 * Implementation of ConfigParser class
 */
public class ConfigParser 
{
    /**
     * Constant tag: comment
     */
    public static final String STRCOMMENT = ";";

    /**
     * Constant tag: key/value association
     */
    public static final String STREQUALS = "=";

    /**
     * Constant tag: Multi-value key tag
     */
    public static final String STRMULTIPREFIX = "*";

    /**
     * Constant tag: Multi-value list separator
     */
    public static final String STRMULTISEPAR = " ";

    /**
     * Constant tag: section label (prefix) separator
     */
    public static final String STRSECTION = "::";
    
    /**
     * Constant tag: token delimiters (list)
     */
    public static final String STRDELIM = " =";

    /**
     * Constant tag: default section label (null=disabled)
     */
    public static final String STRDEFAULTSECTION = null;


    /**
     * Configuration file name
     */
    private String filename;

    /**
     * Configuration lookup table
     */
    Map<String, String> dict;


    /**
     * Default constructor
     */
    public ConfigParser()
    {
        filename = "";
        dict = null;
    }

    /**
     * Constructor with filename
     * @param fname filename to use
     */
    public ConfigParser(String fname)
    {
        filename = fname;
        dict = null;
    }

    /**
     * Copy constructor
     * @param other instance to use as source
     */
    public ConfigParser(ConfigParser other)
    {
        filename = getFilename();
        dict = other.dict;
    }

    /**
     * Return filename in use     
     */
    public String getFilename()
    {
        return (filename);
    }

    /**
     * Retrieve the contents of a key/value configuration file
     * Note: Filename should be set by the appropriate constructor
     */
    public boolean retrieve()
    {
        return (retrieve(filename));
    }

    /**
     * Retrieve the contents of a key/value configuration file
     * @param fname filename to use (internal value reset)
     */
    public boolean retrieve(String fname)
    {
        int lines=0, entries=0, comments=0;

        if (fname == null) {
            System.err.println("Error: ConfigParser.retrieve: Filename is null");
            return (false);
        }
        filename = fname;

        // initialize the lookup table
        if (dict == null) {
            dict = new HashMap<String, String>();
        } else {
            dict.clear();
        }

        // main loop - read the key/value configuration file
        try {
            BufferedReader fread = new BufferedReader(new FileReader(filename));
            String str = null, key=null, value=null, prefix=STRDEFAULTSECTION;
            int pos = -1;
            boolean mvaltag = false;

            while ((str = fread.readLine()) != null) { // readLine() provided by BufferedReader class
                lines++;
                if (str.length()<3) {
                    //System.err.println("Error: ConfigParser.retrieve: Invalid Entry in line " + lines);
                    continue;
                } 

                pos = str.indexOf(STRCOMMENT); // ensured to be at least 3 chars ('k=v')
                if (pos >= 0) {
                    comments++; // comment found
                    if (pos < 4) {
                        //System.err.println("Error: ConfigParser.retrieve: Invalid entry in line " + lines);
                        continue;
                    } else {
                        str = str.substring(0, pos - 1);
                    }
                }

                StringTokenizer tok = new StringTokenizer(str, STRDELIM); // comment-cleaned, valid entry line

                if (tok.countTokens() == 1) { // process as section label
                    str = tok.nextToken().trim();
                    if ((str.startsWith("["))&&(str.endsWith("]"))) {
                        prefix=str.substring(1, str.length()-1);
                        System.out.println("DEBUG: prefix changed to '"+prefix+"'");
                    }
                    else {
                        System.err.println("Error: ConfigParser.retrieve: Invalid section label in line "+lines);
                        continue;
                    }
                }

                if (tok.countTokens()<2) {  // not a section tag, ensure it is a valid key/pair entry
                    System.err.println("Error: ConfigParser.retrieve: Incomplete entry in line "+lines);
                    continue;
                }

                key = tok.nextToken().trim();
                value = tok.nextToken().trim();
                while (tok.hasMoreElements()) {     // retrieve all values (list) for current key
                    mvaltag = true;
                    value = value + STRMULTISEPAR + tok.nextToken().trim();
                }
                if (mvaltag && !key.startsWith(STRMULTIPREFIX)) {
                    System.err.println("Warning: ConfigParser.retrieve: Multi-value entry but key does not start with '"
                            + STRMULTIPREFIX + "' in line " + lines);
                }
                
                entries++;   // everything ok with entry retrieved, insert it to the lookup table
                if (prefix != null) {
                    key = prefix + STRSECTION + key;    // add prefix + separator (if any)
                }
                dict.put(key, value);

                mvaltag = false;
                System.out.println("DEBUG: Valid entry retrieved in line "+lines+": '"+key+"' -> '"+value+"'");
            }

            fread.close();  // all finished
        } catch (IOException e) {
            System.err.println("Error: ConfigParser.retrieve: " + e.toString());
            return (false);
        }

        System.out.println(lines+" lines, "+entries+" entries, "+comments+" comments processed");  // DEBUG: remove if not needed
        System.out.println("dictionary size = "+dict.size());  // DEBUG: remove if not needed
        return (true);
    }


    /**
     * ---  Class main process, used for unit testing ---
     * @param args[] (not used)
     */
    public static void main( String[] args )
    {
        ConfigParser cfg = new ConfigParser("test.cfg");
        cfg.retrieve();
        System.out.println("Lookup table retrieved:\n" + cfg.dict.toString());
        return;
    } // end main

} // end class
