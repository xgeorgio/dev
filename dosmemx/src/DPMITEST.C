/*
    Module:  DPMI capabilities check / version 1.1 / Build: 20220104
    Purpose: Check DPMI server presence in DOS (x86) machines
    Author:  Harris Georgiou (c) 2022, Email:hgeorgiou@unipi.gr
*/

#include <stdio.h>
#include <mem.h>

/*
DPMI: special capabilities check
*/
void dpmi_check( void )
{
	unsigned int i, regax, regbx, regcx, regdx, regdi, reges;
	unsigned long esdi;
	char far * ptr;
	unsigned long buff[32];

	asm {
		mov    ax,0x1687
		int    0x2F
		mov    regax,ax
		mov    regbx,bx
		mov    regcx,cx
		mov    regdx,dx
		mov    regdi,di
		mov    reges,es;
	}

	if (!regax)
		printf("DPMIapi: Not present\n");
	else
	{
		printf("DPMIapi: Detected (%s-bit)[BX=%04x], ver %d.%d[DX=%04x]\n",(((regbx>>8)&1)?"32":"16"),regbx,(regdx>>8),(regdx&0xff),regdx);
		printf("DPMIapi: Callback address is %04X:%04X\n",reges,regdi);
		printf("DPMIapi: Processor type is 80x(%x)86[CX=%04x]\n",(regcx&0xff),regcx);
		asm {
			mov    ax,0x1686
			int    0x2F
			mov    regax,ax
		}
		printf("DPMIapi: Currently operating in %s mode\n",((!regax)?"protected":"real"));

		asm {
			mov    ax,0x0400
			int    0x31
			mov    regbx,bx
			mov    regcx,cx
			mov    regdx,dx
		}
		printf("DPMIapi: %s-bit implementation, refl.int=%s, virt.mem=%s[BX=%04X]\n",
			((regbx&1)?"32":"16"),((regbx&2)?"real":"virt86"),((regbx&1)?"Yes":"No"),regbx);
		printf("DPMIapi: Processor type is 80x(%x)86[CX=%04X]\n",(regcx&0xff),regcx);
		printf("DPMIapi: PIC interrupts master=%02X slave=%02X[DX=%04X]\n",(regdx>>16),(regdx&0xff),regdx);

		asm {
			mov    ax,0x0401
			int    0x31
			mov    reges,es
			mov    regdi,di
		}
		esdi=(unsigned long)reges;
		esdi=(esdi<<16) | regdi;
		ptr=(char far *)esdi;
		printf("DPMIapi: Provider ver %d.%d, %s\n",(unsigned char)ptr[0],(unsigned char)ptr[1],(unsigned char *)ptr[2]);

		reges=((unsigned long)(&buff[0]))>>16;
		regdi=((unsigned long)(&buff[0]))&0xff;
		asm {
			mov    ax,0x050B
			mov    es,reges
			mov    di,regdi
			int    0x31
			mov    regax,ax
		}
		printf("DPMIapi: memory information descriptor\n");
		//for (i=0; i<13; i++)  printf("%lu, ",buff[i]);
		printf("\t%10lu (bytes) : Allocated physical memory controlled by DPMI host\n",buff[0]);
		printf("\t%10lu (bytes) : Allocated virtual memory controlled by DPMI host\n",buff[1]);
		printf("\t%10lu (bytes) : Available virtual memory controlled by DPMI host\n",buff[2]);
		printf("\t%10lu (bytes) : Allocated virtual memory for this virtual machine\n",buff[3]);
		printf("\t%10lu (bytes) : Available virtual memory for this virtual machine\n",buff[4]);
		printf("\t%10lu (bytes) : Allocated virtual memory for this client\n",buff[5]);
		printf("\t%10lu (bytes) : Available virtual memory for this client\n",buff[6]);
		printf("\t%10lu (bytes) : Locked memory for this client\n",buff[7]);
		printf("\t%10lu (bytes) : Maximum locked memory for this client\n",buff[8]);
		printf("\t%10lX : Maximum linear address for this client\n",buff[9]);
		printf("\t%10lu (bytes) : Maximum free block size\n",buff[10]);
		printf("\t%10lu (bytes) : Minimum allocation unit\n",buff[11]);
		printf("\t%10lu (bytes) : Allocation alignment unit\n",buff[12]);
	}
}


int main( void )
{
    dpmi_check();
    printf("\n");

    return(0);
}