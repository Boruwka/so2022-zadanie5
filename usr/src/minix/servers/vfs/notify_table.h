#ifndef _VFS_NOTIFY_TABLE_H
#define _VFS_NOTIFY_TABLE_H

struct notify_wait
{
    struct vnode* file_ptr;
    struct fproc* proc_waiting;
    int event;
} notify_wait[NR_NOTIFY];

int NR_WAITING_FOR_NOTIFY; 

#endif
