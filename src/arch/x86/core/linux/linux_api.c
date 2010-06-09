#include <linux_api.h>

#include <stdarg.h>
#include <asm/unistd.h>
#include <string.h>

int linux_open(const char *pathname, int flags)
{
	return linux_syscall(__NR_open, pathname, flags);
}

int linux_close(int fd)
{
	return linux_syscall(__NR_close, fd);
}

ssize_t linux_read(int fd, void * buf, size_t count)
{
	return linux_syscall(__NR_read, fd, buf, count);
}

ssize_t linux_write(int fd, const void * buf, size_t count)
{
	return linux_syscall(__NR_write, fd, buf, count);
}

int linux_fcntl(int fd, int cmd, ...)
{
	long arg;
	va_list list;

	va_start(list, cmd);
	arg = va_arg(list, long);
	va_end(list);

	return linux_syscall(__NR_fcntl, fd, cmd, arg);
}

int linux_ioctl(int fd, int request, ...)
{
	void * arg;
	va_list list;

	va_start(list, request);
	arg = va_arg(list, void *);
	va_end(list);

	return linux_syscall(__NR_ioctl, fd, request, arg);
}

int linux_poll(struct pollfd * fds, nfds_t nfds, int timeout)
{
	return linux_syscall(__NR_poll, fds, nfds, timeout);
}

int linux_nanosleep(const struct timespec * req, struct timespec * rem)
{
	return linux_syscall(__NR_nanosleep, req, rem);
}

int linux_usleep(useconds_t usec)
{
	struct timespec ts = {
		.tv_sec = (long) (usec / 1000000),
		.tv_nsec = (long) (usec % 1000000) * 1000ul
	};

	return linux_nanosleep(&ts, NULL);
}

int linux_gettimeofday(struct timeval * tv, struct timezone * tz)
{
	return linux_syscall(__NR_gettimeofday, tv, tz);
}

void * linux_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return (void*)linux_syscall(__SYSCALL_mmap, addr, length, prot, flags, fd, offset);
}

void * linux_mremap(void * old_address, size_t old_size, size_t new_size, int flags)
{
	return (void*)linux_syscall(__NR_mremap, old_address, old_size, new_size, flags);
}

int linux_munmap(void * addr, size_t length)
{
	return linux_syscall(__NR_munmap, addr, length);
}
