/*
    Module:  DPMI capabilities check / version 1.1 / Build: 20220104
    Purpose: Check DPMI server presence in DOS (x86) machines
    Author:  Harris Georgiou (c) 2022, Email:hgeorgiou@unipi.gr
*/

#include "xms.c"

int main( void )
{
	XMShandle hid=0;
	unsigned char *buff1=NULL, *buff2=NULL;
	unsigned int  sz=10000, i;


	printf("Extended memory checker (DOS/x86):\n\n");

	xms_init();
	xms_print_info();

	printf("Allocating XMS block of %u bytes\n",sz);
	hid=xms_mem_alloc(sz);
	printf("\thandle=%X (errno=%u: %s)\n",hid,xmsinf.errno,xms_error_str(xmsinf.errno));
	xms_print_handleinfo(hid);
	xms_print_handleinfo(hid+1);

	printf("Allocating conventional buffers of %u bytes\n",sz);
	buff1=(unsigned char *)malloc(sz*sizeof(unsigned char *));
	buff2=(unsigned char *)malloc(sz*sizeof(unsigned char *));
	if (buff1 && buff2)
	{
		for (i=0; i<sz; i++)  buff1[i]=i%10;

		printf("Storing buffer A to XMS\n");
		xms_mem_store(buff1,hid,sz);
		printf("\t(errno=%u: %s)\n",xmsinf.errno,xms_error_str(xmsinf.errno));

		printf("buffer B (prior):\n");
		for (i=0; i<100; i++)  printf("%d",buff2[i]);
		printf("...\n");

		printf("Fetching buffer B from XMS\n");
		xms_mem_fetch(hid,buff2,sz);
		printf("\t(errno=%u: %s)\n",xmsinf.errno,xms_error_str(xmsinf.errno));

		printf("buffer B (after):\n");
		for (i=0; i<100; i++)  printf("%d",buff2[i]);
		printf("...\n");

		if (buff1)  free(buff1);
		if (buff2)  free(buff2);
	}

	printf("Deallocating XMS block\n");
	xms_mem_free(hid);
	printf("\thandle=%X (errno=%u: %s)\n",hid,xmsinf.errno,xms_error_str(xmsinf.errno));

    return(0);
}
