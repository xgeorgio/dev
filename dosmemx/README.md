Title:<br/>
<b>DOSmemX: Extended memory functionality and DPMI server checker for DOS/x86 machines</b>

Description:<br/>
<p>By the beginning of the '90 the personal computers started to have more than the conventional 640 KB of RAM available for user programs. However, DOS did not have native support for memory address space above the first 1024 KB. Moreover, the memory addressing used a segmented 16-bit pair of pointers, making it even more complex to add support e.g. for 4 MB or 8 MB of RAM, which was becoming a norm even for home computers. Native support in DOS-compatible machine emerged only after Microsoft Windows NT and XP came to life, with pure 32-bit memory addressing space in Intel's 386 CPU variants and full support from the popular programming language compilers.</p>
<p>In the meantime, Linux-based machines had no such problem, as it fully exploited 386 and above CPUs from the start with a flat memory addressing space. But DOS machines still remained vastly more popular in home computers and a solution was necessary, especially during the 286 CPU era. Two main solutions were impleneted: Extended Memory Service (XMS) and Expanded Memory Service (EMS), with the first providing memory block management and the second providing a page-based memory management, both implementing internally some form of protected-mode CPU functionality to access memory above the 1024 KB barrier. Additional options became available for DOS programs for using the Upper Memory Block (UMB) and loading device drivers as only hooks pointing to the XMS or EMS memory, thus freeing up more conventional memory. However, programming with XMS or EMS required low-level assembly code and native callbacks to memory addresses, as there was limited to no standard programming interface via DOS or BIOS interrupt calls. There was a third solution, using a full DOS Protected Mode Interface (DPMI) host in 286 and above CPUs, but this required even more complex low-level assembly programming, non-standard interfaces and machine-specific functionalities (286 CPUs did not even have  full protected-mode option or switch back to real-mode).</p>
<p>So, why bother programming an interface for such old machines? The answer is, because they still exist. DOS machines may be a relic of the past, but with software emulators we can develop code for such small platforms that can be used as single-task dedicated controllers, much like the Single-Board Computers (SBC) that exist today. There are extremely easy ways in DOS to handle external I/O such as serial comms port, a mouse, a keyboard, a printer port, even the speaker pins on the motherboards, and everything can be turned to General-Purpose I/O (GPIO) with only few lines of code. At the same time, in order to make something trully useful, memory much larger than 640 KB is usually needed, either for installing a RAM disk in XMS/EMS via device driver for fast file I/O, or directly interacting with XMS/EMS via code for memory allocation above 1024 KB. Between the two main options, XMS is definitely the easier to program, since it requires only memory management in blocks which can be easily swapped to and from heap buffers in the conventional memory area.</p>
<p>In this small library, all the main XMS and UMB functionality is wrapped in proper C-style functions that can be used in any program with a DOS compiler, e.g. Borland's Turbo C++ or similar. <br/>
The functions included are the following (see comments in 'XMS.H' for details):</p>
<ul>
	<li>unsigned char xms_check( void )</li>
	<li>XMScallback xms_get_callback( void )</li>
	<li>void xms_query_info( void )</li>
	<li>void xms_query_avail( void )</li>
	<li>unsigned char xms_init( void )</li>
	<li>void xms_print_info( void )</li>
	<li>XMShandle xms_mem_alloc( unsigned int blocksize )</li>
	<li>unsigned char xms_mem_free( XMShandle handle )</li>
	<li>unsigned char xms_print_handleinfo( XMShandle handle )</li>
	<li>unsigned char xms_mem_store( void far *src, XMShandle dest, unsigned long sz )<li>
	<li>unsigned char xms_mem_fetch( XMShandle src, void far *dest, unsigned long sz )<li>
</ul>

<p>The XMS functionality can be verified with 'XMSTEST.EXE' (e.g. in DOSbox with XMS enabled), which demonstrates XMS memory block fetching.<br/>
Example output from XMSTEST (tested in DOSbox 0.74):</p>
<pre>
Extended memory checker (DOS/x86):

XMSapi: Driver detected, callback address at C83F:0010
XMSapi: Driver version 3.0 (rev 3.1)
XMSapi: Memory available 15168 KB (15168 KB block), HMA not present
Allocating XMS block of 10000 bytes
	handle=1 (errno=0: No error)
Handle=1, locks=0, size=10000 KB (48 handles available)
Allocating conventional buffers of 10000 bytes
Storing buffer A to XMS
Successful XMS store: 10000 bytes from 0348:0C38 to XMS handle 1
	(errno=0: No error)
buffer B (prior):
6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345...
Fetching buffer B from XMS
Successful XMS fetch: 10000 bytes from XMS handle 1 to 0348:5A5C
	(errno=0: No error)
buffer B (after):
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789...
Deallocating XMS block
	handle=1 (errno=0: No error)
</pre>

<p>Since DPMI also provided all the basic XMS functionality, there is also an example 'DPMITEST.EXE', which checks for DPMI server and capabilities. However, its detection success depends on machine type, emulator configuration and any DPMI manager present at the time, since without any such manager it should show "DPMI not present".<br/>
Example output from DPMITEST (tested in DOSbox 0.74):</p>
<pre>
DPMIapi: Detected (16-bit)[BX=0804], ver 8.4[DX=0804]
DPMIapi: Callback address is 0309:0804
DPMIapi: Processor type is 80x(0)86[CX=0200]
DPMIapi: Currently operating in real mode
DPMIapi: 32-bit implementation, refl.int=real, virt.mem=Yes[BX=FFFB]
DPMIapi: Processor type is 80x(89)86[CX=0189]
DPMIapi: PIC interrupts master=00 slave=00[DX=0000]
DPMIapi: Provider ver 0.0, (null)
DPMIapi: memory information descriptor
	         0 (bytes) : Allocated physical memory controlled by DPMI host
	         0 (bytes) : Allocated virtual memory controlled by DPMI host
	         0 (bytes) : Available virtual memory controlled by DPMI host
	         0 (bytes) : Allocated virtual memory for this virtual machine
	         0 (bytes) : Available virtual memory for this virtual machine
	         0 (bytes) : Allocated virtual memory for this client
	         0 (bytes) : Available virtual memory for this client
	         0 (bytes) : Locked memory for this client
	         0 (bytes) : Maximum locked memory for this client
	         0 : Maximum linear address for this client
	         0 (bytes) : Maximum free block size
	         0 (bytes) : Minimum allocation unit
	         0 (bytes) : Allocation alignment unit 
</pre>
