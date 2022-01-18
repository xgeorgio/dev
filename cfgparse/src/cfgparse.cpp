/*
    FILE:           'cfgparse.cpp'
    PACKAGE:        Configuration file parser

    PURPOSE:        Parse and retrieve structured key-value pairs
    VERSION:        1.1
    UPDATED:        18-Jan-2022/14:55

    DESCRIPTION:    The main() implementation can be easily transformed into a lightweight
	                string-based parser of key-value pairs with or without section labels.
					Section labels should be enclosed on brackets '[' and ']', multi-value
					keys should begin with asterisk '*' to flag multiple token parsing,
					comments are marked with ';' and text is skipped therafter.
					See '/examples' folder and corresponding output for capabilities.

    AUTHOR:         Harris Georgiou (c) 2022
*/

#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <string>

#define		STRMAXLEN		1000
#define		STRDELIM		" \t\r\n="
#define     STRSECTIONTAG	"::"


typedef     std::unordered_map<std::string,std::string>  dictionary_t;


unsigned long cfg_parse( const char *filename, dictionary_t &dict )
{
	FILE 	*fh;
	char 	sline[STRMAXLEN], *skey, *sval;    // main string buffers for tokenizer
	char 	smval[STRMAXLEN], prefix[STRMAXLEN], ptemp[STRMAXLEN], ptemp2[STRMAXLEN];    // temp string buffers
	unsigned long  lines=0, entries=0, comments=0, mpos=0;


	if ((fh=fopen(filename,"rb"))==NULL)
	{
		return (-1);
	}
	else    // main parser loop
	{
		dict.clear();			// clear dictionary before new entries are inserted
		strcpy(prefix,"");    	// initialize section label to null
		while(fgets(sline,STRMAXLEN,fh))
		{
			lines++;
			mpos=strlen(sline)-1;
			if (sline[mpos]=='\n')  sline[mpos]='\0';    // remove trailing CR/LF character
			
			skey=strtok(sline,STRDELIM);    // initialize tokenizer for current line
			if (skey)
			{
				if (skey[0]==';')				// comment line detected, skip further text
				{
					comments++;
					continue;
				}
				else if (skey[0]=='[')          // section label detected, handle as key-only
				{
					skey[strlen(skey)-1]='\0';     		// remove trailing ']' character
					strcpy(prefix,&(skey[1]));     		// update prefix with section label...
					strcat(prefix,STRSECTIONTAG);   	// ...and appropriate section separator
					printf("prefix changed to: '%s'\n",prefix);    	// DEBUG: remove if not needed
					continue; 
				}
				
				// normal key-value pair parsing
				//sval=strtok(NULL,STRDELIM);  // warning: NOT working properly after prefix update (overlapping)
				// explicit copy required to avoid overlapping with next 'sval' when 'prefix' has been changed:
				strncpy(ptemp2,strtok(NULL,STRDELIM),STRMAXLEN);   	// copy detected value token to temp buffer...
				sval=ptemp2;                                       	// ...and point the value there
				entries++;
				
				if (skey[0]=='*')		// multi-value detected, parse iteratively to remove delimeters
				{
					mpos=0;
					while(sval)
					{
						if (sval[0]==';')  { comments++; break; }   // skip any comments in multi-value token
						
						strncat(&(smval[mpos]),sval,STRMAXLEN); 	// copy next single-value
						mpos=strlen(smval);                         // update current length
						strcat(&(smval[mpos])," ");  mpos++;    	// add a single space separator
						sval=strtok(NULL,STRDELIM);					// continue with next token
					}
					smval[strlen(smval)-1]='\0';			// tokens finished, terminate multi-value entry
					if (strlen(prefix))						// if available, add section prefix to current key
					{
						strcpy(ptemp,prefix);
						strcat(ptemp,skey);
						strcpy(skey,ptemp);
					}
					printf("\t'%s' = '%s'",skey,smval);		// DEBUG: replace with insertion to dictionary (for lookup)
					dict.emplace(std::make_pair(std::string(skey),std::string(smval)));
					smval[0]='\0';							// reset multi-value buffer for next use
				}
				else		// single-value mode, no further processing required
				{
					if (strlen(prefix))				// if available, add section prefix to current key
					{
						strcpy(ptemp,prefix);
						strcat(ptemp,skey);
						strcpy(skey,ptemp);
					}
					dict.emplace(std::make_pair(std::string(skey),std::string(sval)));
					printf("\t'%s' = '%s'",skey,sval);		// DEBUG: replace with insertion to dictionary (for lookup)
				}
				printf("\n");		// DEBUG: remove if not needed
			}				
		}
		
		fclose(fh);
	}
	
	printf("%lu lines, %lu entries, %lu comments processed\n",lines,entries,comments);	// DEBUG: remove if not needed
	printf("dictionary size = %ld\n",dict.size());		// DEBUG: remove if not needed
	
	return (dict.size());		// on success, return new ditionary size
}


//.....  main testing process  .....

#include <iostream>

int main( int argc, char *argv[] )
{
	dictionary_t    dict;
	
	if (argc!=2)
	{
		printf("Error: Invalid input file\n");
		return (1);
	}
	else if (cfg_parse(argv[1],dict)<0)
	{
		printf("Error: Unable to open input file\n");
		return (2);
	}
	else    // parsing completed, show results from dictionary
	{
		for ( const auto &c : dict )
			printf("key:'%s' -> value:'%s'\n",c.first.c_str(),c.second.c_str());
			//std::cout << c.first << " : " << c.second << '\n';
		
		printf("example lookup: 'anotherkey' -> '%s'\n",dict["anotherkey"].c_str());
		
		return (0);
	}
}
