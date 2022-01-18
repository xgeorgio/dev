/*
    FILE:           'cfgparse.c'
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

#define		STRMAXLEN		256
#define		STRDELIM		" \t\r\n="
#define     STRSECTIONTAG	"::"


int main( int argc, char *argv[] )
{
	FILE 	*fh;
	char 	sline[STRMAXLEN], *skey, *sval;    // main string buffers for tokenizer
	char 	smval[STRMAXLEN], prefix[STRMAXLEN], ptemp[STRMAXLEN], ptemp2[STRMAXLEN];    // temp string buffers
	unsigned long  lines=0, entries=0, comments=0, mpos=0;


	if (argc!=2)
	{
		printf("Error: Invalid input file\n");
		return (1);
	}
	else if ((fh=fopen(argv[1],"rb"))==NULL)
	{
		printf("Error: Unable to open input file\n");
		return (2);
	}
	else    // main parser loop
	{
		strcpy(prefix,"");    // initialize section label to null
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
					printf("\t'%s' = '%s'",skey,sval);		// DEBUG: replace with insertion to dictionary (for lookup)
				}
				printf("\n");		// DEBUG: remove if not needed
			}				
		}
		
		fclose(fh);
	}
	
	printf("%lu lines, %lu entries, %lu comments processed\n",lines,entries,comments);
	
	return (0);
}
