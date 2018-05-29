#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
  int i = 0, j = -1;
  struct proc* curproc = myproc();
  char* virtual_address = (char*) PGROUNDUP(curproc->sz);


  acquire(&(shm_table.lock));
  for (i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id == id) break; // grab the index of the shm_page, if it exists
    if(shm_table.shm_pages[i].id == 0) j = i;  // grab the index of an unused shm_page to use if page doesn't exist.
  }

  if(i == 64) {                      // if the id was not found

    if(j < 0) {
      cprintf("shm_table is out of memory, cannot shm_open()\n");
      release(&(shm_table.lock));
      return -1;
    }
    i = j;                                     // use the unused shm_page
    shm_table.shm_pages[i].id = id;

    char* mem = kalloc();
    if(mem == 0) {
      cprintf("shm_open() out of memory\n");
      release(&(shm_table.lock));
      return -1;                               // error here and return
    }
    memset(mem, 0, PGSIZE);

    shm_table.shm_pages[i].frame = mem;
    shm_table.shm_pages[i].refcnt = 0;
  }
  
  if(mappages(curproc->pgdir, virtual_address, PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W | PTE_U) < 0) {
    cprintf("shm_open() cannot map to virtual memory");
    release(&(shm_table.lock));
    return -1;             // error here and return
  }
  
  shm_table.shm_pages[i].refcnt++;
  curproc->sz = (uint)(virtual_address + PGSIZE);
  *pointer = virtual_address;
  release(&(shm_table.lock));

  return 0; 
}


int shm_close(int id) {
  int i = 0; 

  acquire(&(shm_table.lock));
  for (i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id == id) break; // grab the index of the shm_page, if it exists
  }

  if(i == 64) {
    cprintf("shm_close() : id not found : %d", id);
    release(&(shm_table.lock));
    return -1;
  }

  shm_table.shm_pages[i].refcnt--;
  if(shm_table.shm_pages[i].refcnt == 0) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }

  release(&(shm_table.lock));

return 0; //added to remove compiler warning -- you should decide what to return
}
