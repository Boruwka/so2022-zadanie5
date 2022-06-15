#include "fs.h"
#include "sys/fcntl.h"
#include "file.h"
#include "vnode.h"

struct vnode* find_file_vnode(int fd)
{
    struct filp* filp = get_filp(fd, VNODE_READ);
    if (filp == NULL)
    {
        return NULL;
    }
    struct vnode* vn = filp->filp_vno;
    unlock_filp(filp);
    return vn;
}

int do_notify(void) 
{
    struct vnode* file_ptr = find_file_vnode(m_in.m_lc_vfs_notify.fd);

    if (file_ptr == NULL)
    {
        return EBADF;
    }
    if (m_in.m_lc_vfs_notify.event != NOTIFY_OPEN &&
    m_in.m_lc_vfs_notify.event != NOTIFY_CREATE &&
    m_in.m_lc_vfs_notify.event != NOTIFY_MOVE && 
    m_in.m_lc_vfs_notify.event != NOTIFY_TRIOPEN)
    {
        return EINVAL;
    }
    if (NR_WAITING_FOR_NOTIFY >= NR_NOTIFY)
    {
        return ENONOTIFY;
    }

    if (m_in.m_lc_vfs_notify.event == NOTIFY_CREATE)
    {
        // tu create
        if (!S_ISDIR(file_ptr->v_mode))
        {
            return ENOTDIR;
        }
    }

    if (m_in.m_lc_vfs_notify.event == NOTIFY_MOVE)
    {
        // tu move
        if (!S_ISDIR(file_ptr->v_mode))
        {
            return ENOTDIR;
        }
    }

    if (m_in.m_lc_vfs_notify.event == NOTIFY_TRIOPEN)
    {
        // tu triopen
        if (file_ptr->v_ref_count >= 3)
        {
            return OK;
        }
    }
    
    notify_wait[NR_WAITING_FOR_NOTIFY].file_ptr = file_ptr;
    notify_wait[NR_WAITING_FOR_NOTIFY].proc_waiting = fp;
    notify_wait[NR_WAITING_FOR_NOTIFY].event = m_in.m_lc_vfs_notify.event;
    NR_WAITING_FOR_NOTIFY++;

    suspend(FP_BLOCKED_ON_NOTIFY);
    return SUSPEND;
    return OK;
}
