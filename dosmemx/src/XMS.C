/*
    Module:  DPMI capabilities check / version 1.1 / Build: 20220104
    Purpose: Check DPMI server presence in DOS (x86) machines
    Author:  Harris Georgiou (c) 2022, Email:hgeorgiou@unipi.gr
*/

#include "xms.h"

/*
Convert XMS error codes to error messages
(see XMS.H for details)
*/
const char * xms_error_str( unsigned char xmserrno )
{
	switch(xmserrno)
	{
		case 0x00:
			return("No error");
		case 0x80:
			return("Function not implemented");
		case 0x81:
			return("VDISK device is detected");
		case 0x82:
			return("A20 error detected");
		case 0x90:
			return("HMA does not exist");
		case 0x91:
			return("HMA is already in use");
		case 0x92:
			return("DX is less than the /HMAMIN= parameter");
		case 0xA0:
			return("All extended memory is allocated");
		case 0xA1:
			return("All extended memory handles are in use");
		case 0xA2:
			return("Handle is invalid");
		case 0xA3:
			return("Source handle is invalid");
		case 0xA4:
			return("Source offset is invalid");
		case 0xA5:
			return("Destination handle is invalid");
		case 0xA6:
			return("Destination offset is invalid");
		case 0xA7:
			return("Length is invalid");
		case 0xA8:
			return("Invalid overlap in move request");
		case 0xA9:
			return("Parity error");
		case 0xAB:
			return("Handle is locked");
		case 0xff:
			return("No XMS driver detected");
		default:
			return("Unknown error");
	}
}


/*
    XMS low-level interface to main functions
*/

/*
Check XMS driver (Function 4300h):
(see XMS.H for details)
*/
unsigned char xms_check(void)
{
    unsigned char res = 0;

    asm {
	    mov    ax,0x4300
	    int    0x2F
	    mov    res,al
    }
	if (res!=0x80)
	{
		xmsaddr=NULL;
		xmsinf.errno=0xff;
	}
	else
		xmsinf.errno=0;

    return (xmsinf.errno);
}

/*
Check XMS driver (Function 4310h):
(see XMS.H for details)
*/
XMScallback xms_get_callback( void )
{
    XMScallback  callptr=NULL;
    unsigned int regbx=0, reges=0;
    unsigned long  esbx=0;

    asm {
	    mov    ax,0x4310
	    int    0x2F
	    mov    reges,es
	    mov    regbx,bx
	}

    //printf("callback address = %X:%X\n",reges,regbx);
    esbx=(unsigned long)reges;
    esbx=(esbx<<16) | regbx;
    //printf("callback address = %lx\n",esbx);
    callptr = (XMScallback)esbx;
    //printf("callback address = %Fp\n",callptr);

    return(callptr);
}


/*
Get XMS Version Number (Function 00h):
(see XMS.H for details)
*/
void xms_query_info( void )
{
    unsigned int regax, regdx, regbx;

    if (xmsaddr)
    {
	asm {
	    mov    ah,0x00
	    call   [xmsaddr]
	    mov    regax,ax
	    mov    regbx,bx
	    mov    regdx,dx
	}
	//printf("XMS version: AX=%x(ver), BX=%x(rev), DX=%x(HMA)\n", regax, regbx, regdx);
		xmsinf.vermaj=(regax >> 8);
		xmsinf.vermin=(regax & 0x0f);
		xmsinf.revmaj=(regbx >> 8);
		xmsinf.revmin=(regbx & 0x0f);
		xmsinf.hma=regdx;
		xmsinf.errno=0;
    }
}


/*
Query Free Extended Memory (Function 08h):
(see XMS.H for details)
*/
void xms_query_avail( void )
{
    unsigned int regax, regdx, regbx, res;

    if (xmsaddr)
    {
        asm {
            mov    ah,0x08
            call   [xmsaddr]
            mov    regax,ax
            mov    regdx,dx
            mov    regbx,bx
        }
		xmsinf.errno=regbx & 0xff;
	//printf("XMS available: AX=%04X, DX=%04X, BL=%02X\n", regax, regdx, (regbx & 0xff));
	//printf("XMS available: AX=%u(block KB), DX=%u(total KB), BL=%u(ret)\n", regax, regdx, (regbx & 0xff));
		if (!xmsinf.errno)
		{
			xmsinf.availblk=regax;
			xmsinf.availtot=regdx;
		}
    }
}


/*
Initialize XMS context
(see XMS.H for details)
*/
unsigned char xms_init( void )
{
	if (!xms_check())
	{
		xmsaddr=xms_get_callback();
	xms_query_info();
		xms_query_avail();
	}
	return(xmsinf.errno);
}

/*
Print XMS info and status
(see XMS.H for details)
*/
void xms_print_info( void )
{
	if (!xmsaddr)
		printf("XMSapi: No driver is detected\n");
	else
	{
		printf("XMSapi: Driver detected, callback address at %Fp\n",xmsaddr);
		printf("XMSapi: Driver version %d.%d (rev %d.%d)\n",
			xmsinf.vermaj,xmsinf.vermin,xmsinf.revmaj,xmsinf.revmin);
		printf("XMSapi: Memory available %lu KB (%lu KB block), HMA %spresent\n",
			xmsinf.availtot,xmsinf.availblk,((xmsinf.hma)?"":"not "));
	}
}


/*
Allocate Extended Memory Block (Function 09h):
(see XMS.H for details)
*/
XMShandle xms_mem_alloc( unsigned int blocksize )
{
	unsigned int  regax, regdx, regbx;

    asm {
	mov    ah,0x09
		mov    dx,blocksize
	call   [xmsaddr]
	mov    regax,ax
	mov    regdx,dx
		mov    regbx,bx
    }
	if (regax==1)
	{
		xmsinf.errno=0;
		return(regdx);
	}
	else
	{
		xmsinf.errno=regbx & 0xff;
		return(0);
	}
}

/*
Free Extended Memory Block (Function 0Ah):
(see XMS.H for details)
*/
unsigned char xms_mem_free( XMShandle handle )
{
	unsigned int  regax, regdx, regbx;

    asm {
	mov    ah,0x0A
		mov    dx,handle
	call   [xmsaddr]
	mov    regax,ax
		mov    regbx,bx
    }
	if (regax==1)
		xmsinf.errno=0;
	else
		xmsinf.errno=regbx & 0xff;

	return(xmsinf.errno);
}

/*
Get EMB Handle Information (Function 0Eh):
(see XMS.H for details)
*/
unsigned char xms_print_handleinfo( XMShandle handle )
{
	unsigned int  regax, regdx, regbx;

    asm {
	mov    ah,0x0E
		mov    dx,handle
	call   [xmsaddr]
	mov    regax,ax
		mov    regbx,bx
		mov    regdx,dx
    }
	if (regax==1)
	{
		xmsinf.errno=0;
		printf("Handle=%X, locks=%u, size=%u KB (%u handles available)\n",handle,(regbx>>8),regdx,(regbx&0xff));
	}
	else
		xmsinf.errno=regbx & 0xff;

	return(xmsinf.errno);
}

/*
Move Extended Memory Block (Function 0Bh):
(see XMS.H for details)
*/
unsigned char xms_mem_store( void far *src, XMShandle dest, unsigned long sz )
{
	unsigned int  regax, regbx, regds, regsi;
	XMSmem  xmsmove;

	if (isOdd(sz))  sz=sz+1;
	xmsmove.len=sz;
	xmsmove.src_handle=0;    // source buffer is in conventional memory
	xmsmove.src_ptr=(void far *)src;
	xmsmove.dst_handle=dest;
	xmsmove.dst_ptr=NULL;    // (ignored)
	regds=((unsigned long)((void far *)&xmsmove)) >>16;
	regsi=((unsigned long)((void far *)&xmsmove)) &0xffff;

    asm {
	mov    ah,0x0B
		mov    ds,regds
		mov    si,regsi
	call   [xmsaddr]
	mov    regax,ax
		mov    regbx,bx
    }
	if (regax==1)
	{
		xmsinf.errno=0;
		printf("Successful XMS store: %lu bytes from %Fp to XMS handle %X\n",xmsmove.len,xmsmove.src_ptr,xmsmove.dst_handle);
	}
	else
		xmsinf.errno=regbx & 0xff;

	return(xmsinf.errno);
}


unsigned char xms_mem_fetch( XMShandle src, void far *dest, unsigned long sz )
{
	unsigned int  regax, regbx, regds, regsi;
	XMSmem  xmsmove;

	if (isOdd(sz))  sz=sz+1;
	xmsmove.len=sz;
	xmsmove.src_handle=src;    // source buffer is in XMS memory
	xmsmove.src_ptr=NULL;      // (ignored)
	xmsmove.dst_handle=0;
	xmsmove.dst_ptr=dest;
	regds=((unsigned long)((void far *)&xmsmove)) >>16;
	regsi=((unsigned long)((void far *)&xmsmove)) &0xffff;

    asm {
	mov    ah,0x0B
		mov    ds,regds
		mov    si,regsi
	call   [xmsaddr]
	mov    regax,ax
		mov    regbx,bx
    }
	if (regax==1)
	{
		xmsinf.errno=0;
		printf("Successful XMS fetch: %lu bytes from XMS handle %X to %Fp\n",xmsmove.len,xmsmove.src_handle,xmsmove.dst_ptr);
	}
	else
		xmsinf.errno=regbx & 0xff;

	return(xmsinf.errno);
}

