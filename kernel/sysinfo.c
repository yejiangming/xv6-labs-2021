
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"
#include "sysinfo.h"


uint64 sys_sysinfo(void) {
    uint64 addr;
    if (argaddr(0, &addr) < 0) {
        return -1;
    };

    struct sysinfo info;

    info.freemem = kfree_count() * PGSIZE;
    info.nproc = proc_count();

    struct proc *p = myproc();
    if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0) {
        return -1;
    }

    return 0;
}