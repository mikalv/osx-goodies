#if __cplusplus < 201103L
    #error "I need C++11 for functional jokery ;)"
#endif

#include "RAID.h"

static struct cdevsw mraid_cdevsw = {
    RAID::MRAID_Open, // lambda(dev_t, int d_open, int, struct proc *) { return d_open; }
    RAID::MRAID_Close, // lambda(dev_t, int d_close, int, struct proc *) { return d_close; }
    (d_read_t *)&nulldev, // lambda(dev_t, struct uio *, int d_read) { return d_read; }
    (d_write_t *)&nulldev, // lambda(dev_t, struct uio *, int d_write) { return d_write; }
    RAID::MRAID_Ioctl, // lambda(dev_t, u_long, caddr_t, int d_ioctl, struct proc *) { return d_ioctl; }
    (d_stop_t *)&nulldev, // lambda(struct tty*, int d_stop) { return d_stop; }
    (d_reset_t *)&nulldev, // lambda(int d_reset) { return d_reset; }
    0,               // struct tty      **d_ttys;
    (d_select_t *)&nulldev, // lambda(dev_t, int d_select, void *, struct proc *) { return d_select; }
    eno_mmap,        // mmap_fcn_t       *d_mmap;
    eno_strat,       // strategy_fcn_t   *d_strategy;
    eno_getc,        // getc_fcn_t       *d_getc;
    eno_putc,        // putc_fcn_t       *d_putc;
    D_TTY,           // int               d_type;
};

RAID::~RAID() {
    int ret;
    
    if (raid_devnode != NULL) devfs_remove(raid_devnode);
    if (devindex != -1) {
        ret = cdevsw_remove(devindex, &mraid_cdevsw);
        if (ret != devindex)
            DbgPrint("[RAID] cdevsw_remove() failed (returned %d)\n", ret);
    }
}

bool RAID::init(SInt32 domain)
{
    char str[5];
    
    if ((devindex = cdevsw_add(-1, &mraid_cdevsw)) == -1) {
        IOPrint("[RAID] cdevsw_add() failed\n");
        return false;
    }
    
    snprintf(str, sizeof(str), "mfi%d", domain);
    
    if ((raid_devnode = devfs_make_node(makedev(DEVINDEX, 0),
                                     DEVFS_CHAR,
                                     UID_ROOT,
                                     GID_OPERATOR,
                                     0640,
                                     str)) == NULL)
        return false;
    
    return true;
}

int RAID::MRAID_Ioctl(__unused dev_t dev, u_long cmd, __unused caddr_t data,
           __unused int flag, __unused struct proc *p)
{
    switch (cmd) {
        default:
            DbgPrint("[RAID] Ioctl 0x%lx not handled\n", cmd);
            return ENOTTY;
    }
}

int RAID::MRAID_Open(__unused dev_t dev, __unused int, __unused int, __unused struct proc *p)
{
    return 0;
}
int RAID::MRAID_Close(__unused dev_t dev, __unused int, __unused int, __unused struct proc *p)
{
    return 0;
}