/*
    Module:  Extended memory (XMS) services / version 1.1 / Build: 20220104
    Purpose: Wrapper functions for XMS in DOS (x86) machines
    Author:  Harris Georgiou (c) 2022, Email:hgeorgiou@unipi.gr
*/

#ifndef		_XMS_H
#define		_XMS_H

#include <stdio.h>
#include <mem.h>

/*
    XMS type definitions for context and global variables
*/
typedef void far *   XMScallback;    // main callback address for XMS driver

typedef unsigned int XMShandle;      // memory block handle

typedef struct {                     // callback context (registers)
    unsigned short ax, dx, bx;
    void far * ds_si;
    }  XMScontext;

typedef union {		             // either 32-bit offset or real-mode pointer
	unsigned long offset;
	void far * ptr;
    }  XMSptr;

typedef struct {                      // context for versioning and error code
    unsigned short vermaj, vermin, revmaj, revmin, hma;
	unsigned long  availblk, availtot;
    unsigned short errno;
    } XMSinfo;

typedef struct {		              // contxt for memory block move/copy
	unsigned long len;
	XMShandle  src_handle;
	void far * src_ptr;
	XMShandle  dst_handle;
	void far * dst_ptr;
    }  XMSmem;

#define isOdd(X)	(((X) & 1L) != 0)     // check for odd=valued offset


/*
    XMS global variables definitions
*/
XMScallback  xmsaddr = NULL;
XMScontext   xmsctx;
XMSinfo      xmsinf;

/*
Convert XMS error codes to error messages

    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = 82h if an A20 error occurs
	    BL = 90h if the HMA does not exist
	    BL = 91h if the HMA is already in use
	    BL = 92h if DX is less than the /HMAMIN= parameter
	    BL = A0h if all extended memory is allocated
	    BL = A1h if all available extended memory handles are in use
	    BL = A2h if the handle is invalid
	    BL = A3h if the SourceHandle is invalid
	    BL = A4h if the SourceOffset is invalid
	    BL = A5h if the DestHandle is invalid
	    BL = A6h if the DestOffset is invalid
	    BL = A7h if the Length is invalid
	    BL = A8h if the move has an invalid overlap
	    BL = A9h if a parity error occurs
	    BL = ABh if the handle is locked

*/
const char * xms_error_str( unsigned char xmserrno );

/*
    XMS low-level interface to main functions
*/

/*
Check XMS driver (Function 4300h):
----------------------------------
The recommended way of determining if an XMS driver is installed
is to set AH=43h and AL=00h and then execute INT 2Fh.  If an XMS
driver is available, 80h will be returned in AL.

    Example:
	    ; Is an XMS driver installed?
	    mov     ax,4300h
	    int     2Fh
	    cmp     al,80h
	    jne     NoXMSDriver
*/
unsigned char xms_check( void );

/*
Check XMS driver (Function 4310h):
----------------------------------
Programs can execute INT 2Fh with AH=43h and AL=10h to obtain the
address of the driver's control function.  The address is returned
in ES:BX.  This function is called to access all of the XMS functions.
It should be called with AH set to the number of the API function
requested.  The API function will put a success code of 0001h or 0000h
in AX.  If the function succeeded (AX=0001h), additional information
may be passed back in BX and DX.  If the function failed (AX=0000h),
an error code may be returned in BL.  Valid error codes have their
high bit set.  Developers should keep in mind that some of the XMS API
functions may not be implemented by all drivers and will return failure
in all cases.

    Example:
	    ; Get the address of the driver's control function
	    mov     ax,4310h
	    int     2Fh
	    mov     word ptr [XMSControl],bx      ;XMSControl is a DWORD
	    mov     word ptr [XMSControl+2],es

	    ; Get the XMS driver's version number
	    mov     ah,00h
	    call    [XMSControl]    ; Get XMS Version Number

    NOTE: Programs should make sure that at least 256 bytes of
	  stack space is available before calling XMS API functions.
*/
XMScallback xms_get_callback( void );

/*
Get XMS Version Number (Function 00h):
--------------------------------------

    ARGS:   AH = 00h
    RETS:   AX = XMS version number
	    BX = Driver internal revision number
	    DX = 0001h if the HMA exists, 0000h otherwise
    ERRS:   None

    This function returns with AX equal to a 16-bit BCD number
representing the revision of the DOS Extended Memory Specification
which the driver implements (e.g. AX=0235h would mean that the
driver implemented XMS version 2.35).  BX is set equal to the
driver's internal revision number mainly for debugging purposes.
DX indicates the existence of the HMA (not its availability) and
is intended mainly for installation programs.

    NOTE: This document defines version 3.00 of the specification.
*/
void xms_query_info( void );

/*
Query Free Extended Memory (Function 08h):
------------------------------------------

    ARGS:   AH = 08h
    RETS:   AX = Size of the largest free extended memory block
		 in K-bytes
	    DX = Total amount of free extended memory in K-bytes
    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = A0h if all extended memory is allocated

    This function returns the size of the largest available extended memory
block in the system.
*/
void xms_query_avail( void );

/*
Initialize XMS context
*/
unsigned char xms_init( void );

/*
Print XMS info and status
*/
void xms_print_info( void );

/*
Allocate Extended Memory Block (Function 09h):
----------------------------------------------

    ARGS:   AH = 09h
	    DX = Amount of extended memory being requested in K-bytes
    RETS:   AX = 0001h if the block is allocated,
		 0000h otherwise
	    DX = 16-bit handle to the allocated block
    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = A0h if all available extended memory is allocated
	    BL = A1h if all available extended memory handles
		 are in use

    This function attempts to allocate a block of the given
size out of the pool of free extended memory.  If a block is available,
it is reserved for the caller and a 16-bit handle to that block is
returned.  The handle should be used in all subsequent extended memory
calls.  If no memory was allocated, the returned handle is null.

    NOTE: Extended memory handles are scarce resources. Programs
	  should try to allocate as few as possible at any one time.
	  When all of a driver's handles are in use, any free
	  extended memory is unavailable.
*/
XMShandle xms_mem_alloc( unsigned int blocksize );

/*
Free Extended Memory Block (Function 0Ah):
------------------------------------------

    ARGS:   AH = 0Ah
	    DX = Handle to the allocated block which should be freed
    RETS:   AX = 0001h if the block is successfully freed,
		 0000h otherwise
    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = A2h if the handle is invalid
	    BL = ABh if the handle is locked

    This function frees a block of extended memory which was
previously allocated using Function 09h (Allocate Extended Memory
Block).  Programs which allocate extended memory should free their
memory blocks before exiting.  When an extended memory buffer is freed,
its handle and all data stored in it become invalid and should not be
accessed.
*/
unsigned char xms_mem_free( XMShandle handle );

/*
Get EMB Handle Information (Function 0Eh):
------------------------------------------

    ARGS:   AH = 0Eh
	    DX = Extended memory block handle
    RETS:   AX = 0001h if the block's information is found,
		 0000h otherwise
	    BH = The block's lock count
	    BL = Number of free EMB handles in the system
	    DX = The block's length in K-bytes
    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = A2h if the handle is invalid

    This function returns additional information about an extended
memory block to the caller.

    NOTE: To get the block's base address, use Function 0Ch
	  (Lock Extended Memory Block).
*/
unsigned char xms_print_handleinfo( XMShandle handle );

/*
Move Extended Memory Block (Function 0Bh):
------------------------------------------

    ARGS:   AH = 0Bh
	    DS:SI = Pointer to an Extended Memory Move
		    Structure (see below)
    RETS:   AX = 0001h if the move is successful,
		 0000h otherwise
    ERRS:   BL = 80h if the function is not implemented
	    BL = 81h if a VDISK device is detected
	    BL = 82h if an A20 error occurs
	    BL = A3h if the SourceHandle is invalid
	    BL = A4h if the SourceOffset is invalid
	    BL = A5h if the DestHandle is invalid
	    BL = A6h if the DestOffset is invalid
	    BL = A7h if the Length is invalid
	    BL = A8h if the move has an invalid overlap
	    BL = A9h if a parity error occurs

    Extended Memory Move Structure Definition:

     ExtMemMoveStruct    struc
       Length              dd  ? ; 32-bit number of bytes to transfer
       SourceHandle        dw  ? ; Handle of source block
       SourceOffset        dd  ? ; 32-bit offset into source
       DestHandle          dw  ? ; Handle of destination block
       DestOffset          dd  ? ; 32-bit offset into destination block
       ExtMemMoveStruct    ends

    This function attempts to transfer a block of data from one
location to another.  It is primarily intended for moving blocks of
data between conventional memory and extended memory, however it can
be used for moving blocks within conventional memory and within
extended memory.

    NOTE: If SourceHandle is set to 0000h, the SourceOffset is
	  interpreted as a standard segment:offset pair which refers
	  to memory that is directly accessible by the processor.
	  The segment:offset pair is stored in Intel DWORD notation.
	  The same is true for DestHandle and DestOffset.

          SourceHandle and DestHandle do not have to refer to
          locked memory blocks.

	  Length must be even. Although not required, WORD-aligned moves 
	      can be significantly faster on most machines.
          DWORD aligned move can be even faster on 80386 machines.

	  If the source and destination blocks overlap, only
	  forward moves (i.e. where the source base is less than the
	  destination base) are guaranteed to work properly.

	  Programs should not enable the A20 line before calling
	  this function. The state of the A20 line is preserved.

	  This function is guaranteed to provide a reasonable
	  number of interrupt windows during long transfers.
*/
unsigned char xms_mem_store( void far *src, XMShandle dest, unsigned long sz );

unsigned char xms_mem_fetch( XMShandle src, void far *dest, unsigned long sz );

#endif
