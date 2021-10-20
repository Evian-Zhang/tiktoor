# TIKTOOR

> TIKTOOR DID I DWELL， LEWD I DID ROOTKIT

## 通信协议

client与lkm通过`ioctl`通信。client生成一个socket fd后，对该fd发出`ioctl`，lkm对`inet_ioctl`进行hook，进行通信。这种方案参考了[f0rb1dd3n/Reptile](https://github.com/f0rb1dd3n/Reptile)，实际上也可以通过`kill`等系统调用进行通信，但这种方案可以传递自定义参数，更加灵活。

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

### 内核模块隐藏

`action`为`0x5`。

完整的rootkit需要将自身隐藏。我们实现了这个功能，但为了演示的需要，并不是在模块载入的时候自动隐藏，而是收到命令后将自身隐藏。

### 内核模块恢复

`action`为`0x6`。

## 原理

### 进程隐藏

参考[f0rb1dd3n/Reptile](https://github.com/f0rb1dd3n/Reptile)。

将`pid`对应的`struct task_struct`的`flag`字段增加位`0x02000000`（从linux源码来看，目前这个标志位没有被占用）。

首先，我们对`find_task_by_vpid`进行hook。该API是所有通过pid获取进程的入口。通过hook，可以阻止他人通过pid获取被隐藏的进程。

然后，我们对`vfs_statx`进行hook。该API是内部API，用于对文件系统的访问。通过hook，可以阻止他人通过`/proc`虚拟文件系统操作进程。

此外，我们对`copy_creds`和`exit_creds`进行hook，确保被隐藏的进程`fork()`出的子进程依然被隐藏。

### 模块隐藏

参考<https://xcellerator.github.io/posts/linux_rootkits_05/>，只需要将当前的模块在模块列表中删除即可（由于没有被释放，所以当前模块仍然在内核内存中工作）
