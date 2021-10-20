# TIKTOOR

> TIKTOOR DID I DWELL， LEWD I DID ROOTKIT

## 通信协议

client与lkm通过`ioctl`通信。client生成一个socket fd后，对该fd发出`ioctl`，lkm对`inet_ioctl`进行hook，进行通信。

`ioctl`原型：

```c
#include <sys/ioctl.h>
int ioctl(int fd, unsigned long request, ...);
```

其具体参数，根据`fd`的实际类型有关。如果`fd`是socket fd，则内核态的`inet_ioctl`响应函数原型为

```c
int inet_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg);
```

我们通过定义与现有常数不冲突的`cmd`作为rootkit标识，hook该函数，对有该标识的请求进行特殊处理。

rootkit的`cmd`标识符为`0xdeadbeaf`，此时`arg`为指针，指向结构体

```c
struct TiktoorCmdArg {
    unsigned char action;
    void *subargs;
};
```

`subargs`指向的实际类型根据`action`的值而定。

### 驱动隐藏

`action`为`0x0`

### 文件隐藏

`action`为`0x1`

### 端口隐藏

`action`为`0x2`

### 进程隐藏

`action`为`0x3`。

此时`subargs`指向的类型为

```c
struct ProcessHidingSubargs {
    unsigned int pid;
};
```

### 进程保护

`action`为`0x4`
