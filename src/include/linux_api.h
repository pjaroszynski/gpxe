#ifndef _LINUX_API_H
#define _LINUX_API_H

#include <bits/linux_api.h>
#include <bits/linux_api_platform.h>

#include <stdint.h>

typedef int pid_t;

#include <linux/types.h>
#include <linux/posix_types.h>
#include <linux/time.h>
#include <linux/mman.h>
#include <linux/fcntl.h>
#include <linux/ioctl.h>
#include <linux/poll.h>

typedef long useconds_t;

extern long linux_syscall(int number, ...);

extern int linux_open(const char *pathname, int flags);
extern int linux_close(int fd);
extern ssize_t linux_read(int fd, void * buf, size_t count);
extern ssize_t linux_write(int fd, const void * buf, size_t count);
extern int linux_fcntl(int fd, int cmd, ...);
extern int linux_ioctl(int fd, int request, ...);

typedef unsigned long nfds_t;
extern int linux_poll(struct pollfd * fds, nfds_t nfds, int timeout);

extern int linux_nanosleep(const struct timespec * req, struct timespec * rem);
extern int linux_usleep(useconds_t usec);
extern int linux_gettimeofday(struct timeval * tv, struct timezone * tz);

#define MAP_FAILED ((void *)-1)

extern void * linux_mmap(void * addr, size_t length, int prot, int flags, int fd, off_t offset);
extern void * linux_mremap(void * old_address, size_t old_size, size_t new_size, int flags);
extern int linux_munmap(void * addr, size_t length);

extern const char * linux_strerror(int errnum);

#endif /* _LINUX_API_H */
