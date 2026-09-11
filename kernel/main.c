#define WAKS_TYPE_IMPLEMENTATION
#define WAKS_ARCH_IMPLEMENTATION
#define WAKS_ALLOCATOR_IMPLEMENTATION
#define WAKS_CONTAINER_IMPLEMENTATION
#define WAKS_IO_IMPLEMENTATION

#define WAKS_TARGET_BAREMETAL
#include "waks.h"

// ## Scope boundary
// *How do I transition the CPU from 32-bit protected mode (where Multiboot leaves me) 
// into 64-bit long mode, before calling `kernel_main`?*
// 
// What boot.s needs to do, in order:
//    [x] 1. Set up minimal page tables: 
//           long mode requires paging to be enabled; you need at least a PML4, one PDPT,
//           and enough PD/PT entries to identity-map the first few MB (where your kernel actually lives)
//    [x] 2. Enable PAE — set bit 5 of `CR4`
//    [x] 3. Load `CR3`: with the physical address of your PML4 table
//    [ ] 4. Enable long mode:
//             set the `LME` bit (bit 8) in the `EFER` MSR via `wrmsr`
//    [ ] 5. Enable paging:
//             set the `PG` bit (bit 31) of `CR0` — this is the instant the CPU actually enters long mode
//             (technically "compatibility mode" until the next step)
//    [ ] 6. Load a 64-bit GDT:
//             and far-jump into a 64-bit code segment, this is what finally gets you into true 64-bit mode
//    [ ] 7. Only now:
//         set up a 64-bit stack and call `kernel_main`
// 
// Research trace (same source-of-truth order as before):
//    1. OSDev Wiki: "Setting Up Long Mode":
//         this is the canonical page for exactly this transition, written specifically for the Multiboot-then-long-mode scenario you're in
//    2. OSDev Wiki: "Paging":
//         for understanding PML4/PDPT/PD/PT structure before writing the page table setup
//    3. Intel/AMD manuals (or a summary of them): 
//         for the exact `CR0`/`CR3`/`CR4`/`EFER` bit layouts, if you want to go to the real source of truth rather than trusting OSDev's summary alone
//    4. A reference implementation:
//         many toy 64-bit kernel tutorials include a working long-mode boot.s you can compare against once you've written your own attempt, the same way you cross-checked the hashmap against klib/stb_ds
// 
// ## Suggested split
// This is a meaningfully sized chunk of new work — probably deserves its own research session before implementation, matching the UART pacing (5 hours reading before writing `arch.h`). Given today's already been a long debugging stretch (target triples, architecture mismatches, register errors), is this the next session's focus, or do you want to start the research now while it's fresh?


// What is PML4, PDPT, PD, and PT: 
// Four hierarchical levels of paging structures in x86-64 architecture used to translate virtual 
// addresses into physical addresses.  
// 
// allows the system to manage large memory spaces efficiently by only allocating page table entries for used address ranges. 
// 
// The Page Map Level 4 (PML4): is the root of the hierarchy, with its base address stored in the CR3 CPU register.
//    Each entry in the PML4 points to a Page Directory Pointer Table (PDPT). 
//    Each entry in the PDPT points to a Page Directory (PD). 
//    Finally, each entry in the PD points to a Page Table (PT).  
// Entries in the PT contain the physical address of the actual 4 KB memory pages. 
// 
//     Bits 47 – 39 index the PML4. 
//     Bits 38 – 30 index the PDPT. 
//     Bits 29 – 21 index the PD. 
//     Bits 20 – 12 index the PT. 
//     Bits 11 –  0 provide the offset within the final physical page.
// each table consists of 512 entries (8 bytes each, totaling 4 KB), with each 
// entry providing a Page Frame Number (PFN) that points to the next level’s table or the final data page. 

void kernel_main(void)
{
    // Serial output (for your 'display none' terminal)
    waks_io_print(WAKS_2STR("\n[WaksOS] Kernel Loaded Successfully.\n"));
    waks_io_print(WAKS_2STR("[WaksOS] Silicon Block: 6 Hours Remaining.\n"));

    for (;;) {
        __asm__("hlt");
    }
}
