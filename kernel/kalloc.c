// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

   // index of a physical page in the reference-count array
   #define PA2REF(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

   struct {
     struct spinlock lock;
     struct run *freelist;
     int ref[(PHYSTOP - KERNBASE) / PGSIZE]; // how many page tables use each page
   } kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void *)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
       for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE) {
       kmem.ref[PA2REF(p)] = 1; // so kfree() drops it to 0 and frees it
       kfree(p);
     }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  
       // only really free the page when no page table uses it any more.
     acquire(&kmem.lock);
     if (kmem.ref[PA2REF(pa)] <= 0)
       panic("kfree: ref");
     kmem.ref[PA2REF(pa)]--;
     if (kmem.ref[PA2REF(pa)] > 0) {
       release(&kmem.lock);
       return;
     }
     release(&kmem.lock);
       // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
      if (r) {
       kmem.freelist = r->next;
       kmem.ref[PA2REF(r)] = 1;
     }
  release(&kmem.lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}

// one more page table now uses physical page pa (copy-on-write fork)
void
kaddref(void *pa)
{
  acquire(&kmem.lock);
  kmem.ref[PA2REF(pa)]++;
  release(&kmem.lock);
}