Title:<br/>
<b>DOSmemX: Extended memory functionality and DPMI server checker for DOS/x86 machines</b>

Description:<br/>

Example output from XMSTEST (tested in DOSbox 0.74):
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

Example output from DPMITEST (tested in DOSbox 0.74):
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

