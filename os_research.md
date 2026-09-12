What is PML4, PDPT, PD, and PT: 
   Four hierarchical levels of paging structures in x86-64 architecture used to translate virtual 
   addresses into physical addresses.  
allows the system to manage large memory spaces efficiently by only allocating page table entries for used address ranges. 

The Page Map Level 4 (PML4): is the root of the hierarchy, with its base address stored in the CR3 CPU register.
   Each entry in the PML4 points to a Page Directory Pointer Table (PDPT). 
   Each entry in the PDPT points to a Page Directory (PD). 
   Finally, each entry in the PD points to a Page Table (PT).  
Entries in the PT contain the physical address of the actual 4 KB memory pages. 

    Bits 47 – 39 index the PML4. 
    Bits 38 – 30 index the PDPT. 
    Bits 29 – 21 index the PD. 
    Bits 20 – 12 index the PT. 
    Bits 11 –  0 provide the offset within the final physical page.
 each table consists of 512 entries (8 bytes each, totaling 4 KB), with each 
 entry providing a Page Frame Number (PFN) that points to the next level’s table or the final data page. 

 What boot.s needs to do, in order:
  [x] Set up minimal page tables: 
        long mode requires paging to be enabled; you need at least a PML4, one PDPT,
        and enough PD/PT entries to identity-map the first few MB (where your kernel actually lives)
  [x] Enable PAE — set bit 5 of `CR4`
  [x] Load `CR3`: with the physical address of your PML4 table
  [x] Enable long mode:
        set the `LME` bit (bit 8) in the `EFER` MSR via `wrmsr`
  [x] Enable paging:
        set the `PG` bit (bit 31) of `CR0` — this is the instant the CPU actually enters long mode
        (technically "compatibility mode" until the next step)
  [x] Load a 64-bit GDT:
        and far-jump into a 64-bit code segment, this is what finally gets you into true 64-bit mode
  [x] Only now:
         set up a 64-bit stack and call `kernel_main`


- [] IDT:
   Scope boundary for IDT
      How do I set up an IDT so the CPU can call my handler code when an interrupt or exception occurs, in 64-bit long mode?
   
   What's in scope
     - [] IDT structure in long mode: 
           how it differs from the 32-bit IDT (64-bit mode has a wider gate descriptor format,
           16 bytes instead of 8, since handler addresses are now 64-bit)
     - [] Gate descriptor format: 
            what fields make up each of the 256 possible entries (offset, segment selector, type/attributes)
     - [] idt instruction: 
            the direct analog of lgdt from yesterday, loading the IDT's address/size
     - [] A minimal handler stub: 
            what a bare "do nothing but return" interrupt handler needs (specifically iretq for the return,
            not a normal ret, since interrupts have different stack behavior)
     - [] Testing it: 
            deliberately triggering a fault (divide by zero is the classic first test) to confirm your handler actually gets called
   Suggested research trace
       OSDev Wiki — "Interrupt Descriptor Table" (the direct analog of yesterday's "Setting Up Long Mode" page — likely your primary source)
       OSDev Wiki — "Exceptions" (for understanding what CPU-generated faults look like, which ones exist, and their vector numbers)


