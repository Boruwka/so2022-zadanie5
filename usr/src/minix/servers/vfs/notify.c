#include "fs.h"
#include "sys/fcntl.h"
#include "file.h"
#include "vnode.h"

struct vnode* find_file_vnode(int fd)
{
    //printf("bede szukac vnoda\n");
    struct filp* filp = get_filp(fd, VNODE_READ);
    if (filp == NULL)
    {
        //printf("filp to null\n");
        return NULL;
    }
    struct vnode* vn = filp->filp_vno;
    unlock_filp(filp);
    return vn;
}

int do_notify(void) 
{
    //printf("witamy w do_notify\n");
    struct vnode* file_ptr = find_file_vnode(m_in.m_lc_vfs_notify.fd);
    //printf("vnode uzyskany\n");

    if (file_ptr == NULL)
    {
        printf("nie da sie uzyskac wskaznika do tego pliku\n");
        return EBADF;
    }
    if (m_in.m_lc_vfs_notify.event != NOTIFY_OPEN)
    {
        // na razie tylko open obsługujemy
        printf("zly deskryptor eventu\n");
        return EINVAL;
    }
    // na razie olejemy ENOTDIR
    if (NR_WAITING_FOR_NOTIFY >= NR_NOTIFY)
    {
        printf("za duzo procesow czeka\n");
        return ENONOTIFY;
    }
    
    //printf("bede sie dodawac\n");
    notify_wait[NR_WAITING_FOR_NOTIFY].file_ptr = file_ptr;
    notify_wait[NR_WAITING_FOR_NOTIFY].proc_waiting = fp;
    notify_wait[NR_WAITING_FOR_NOTIFY].event = m_in.m_lc_vfs_notify.event;
    NR_WAITING_FOR_NOTIFY++;
    //printf("Dodalem sie i ide spac\n");

    suspend(FP_BLOCKED_ON_NOTIFY);
    return SUSPEND;
    return OK;
}
