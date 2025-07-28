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

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

int pa_ref[(PHYSTOP-KERNBASE)/PGSIZE]; // reference count for each physical page

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
  memset(pa_ref, 0, sizeof(pa_ref)); // initialize reference counts to 0
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  int ref_index = pa_ref_index((uint64)pa);
  acquire(&kmem.lock);
  if (pa_ref[ref_index] > 1) {
    pa_ref[ref_index]--;
    release(&kmem.lock);
    return; // do not free if reference count is greater than 1
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  r->next = kmem.freelist;
  kmem.freelist = r;

  pa_ref[ref_index] = 0; // reset reference count to 0

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
  if(r) {
    kmem.freelist = r->next;
    uint64 pa = (uint64)r;
    int ref_index = pa_ref_index(pa);
    pa_ref[ref_index] = 1;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

int pa_ref_index(uint64 pa)
{
  if (pa < KERNBASE || pa >= PHYSTOP)
    return -1; // invalid address
  return (pa - KERNBASE) / PGSIZE;
}

int increment_ref(uint64 pa)
{
  int ref_index = pa_ref_index(pa);
  if (ref_index == -1) {
    panic("increment_ref: invalid address");
    return -1; // invalid address
  }

  int index = -1;
  acquire(&kmem.lock);
  index = pa_ref[ref_index]++;
  release(&kmem.lock);
  return index;
}