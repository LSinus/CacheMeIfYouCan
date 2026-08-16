#define _GNU_SOURCE
#include "utils.h"
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#define EXIT_FAILURE -1

/* Ensure that kpti is enabled and pcid is disabled to make this function work */ 
void flush_tlb() {
    open("0asdawdawdawd", O_RDWR); // invalid syscall tlb flush with kpti enabled and pcid disabled  
}


static bool write_file(const char *file, const char *what, ...) {
    char buf[1024];
    va_list args;
    va_start(args, what);
    vsnprintf(buf, sizeof(buf), what, args);
    va_end(args);
    buf[sizeof(buf) - 1] = 0;
    int len = strlen(buf);
    int fd = open(file, O_WRONLY | O_CLOEXEC);
    if (fd == -1)
        return false;
    if (write(fd, buf, len) != len) {
        int err = errno;
        close(fd);
        errno = err;
        return false;
    }
    close(fd);
    return true;
}

void setup_namespace() {
    int real_uid = getuid();
    int real_gid = getgid();

    if (unshare(CLONE_NEWUSER) != 0) {
        perror("[-] unshare(CLONE_NEWUSER)");
        _exit(EXIT_FAILURE);
    }

    if (unshare(CLONE_NEWNET) != 0) {
        perror("[-] unshare(CLONE_NEWUSER)");
        _exit(EXIT_FAILURE);
    }

    if (!write_file("/proc/self/setgroups", "deny")) {
        perror("[-] write_file(/proc/self/set_groups)");
        _exit(EXIT_FAILURE);
    }
    if (!write_file("/proc/self/uid_map", "0 %d 1\n", real_uid)) {
        perror("[-] write_file(/proc/self/uid_map)");
        _exit(EXIT_FAILURE);
    }
    if (!write_file("/proc/self/gid_map", "0 %d 1\n", real_gid)) {
        perror("[-] write_file(/proc/self/gid_map)");
        _exit(EXIT_FAILURE);
    }
}
