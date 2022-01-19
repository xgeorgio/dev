/*    ******************************************************************
      ***                                                            ***
      ***         Artificial Neural Networks - Kohonen model         ***
      ***         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~         ***
      ***                                                            ***
      ***                                                            ***
      ******************************************************************


        DESCRIPTION:
                        
            This file contains constant definitions of exit codes, used
          by the various (user) programs. 


------------------------------------------------------------------------------
*/

/* Exit code constants for user programs */

#ifndef     _USER_EXITCODE_H
#define     _USER_EXITCODE_H

#define         EXIT_NOERR          0       /* No error (normal exit) */
#define         EXIT_USERBRK        -1      /* User break (Ctrl+C) */
#define         EXIT_FLTPERR        -2      /* Floating-point error */
#define         EXIT_MEMVERR        -3      /* Memory violation error */
#define         EXIT_ARGVERR        -4      /* Invalid arguments used */
#define         EXIT_PARSERR        1       /* Parsing error (asm.) */
#define         EXIT_CODEERR        2       /* Code structure error (asm.) */
#define         EXIT_FILEERR        3       /* File I/O error */

#endif

