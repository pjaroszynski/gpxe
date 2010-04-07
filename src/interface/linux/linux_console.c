#include <console.h>

#include <gpxe/init.h>
#include <gpxe/keys.h>
#include <linux_api.h>

#include <linux/termios.h>
#include <asm/errno.h>

static void linux_console_putchar(int c)
{
	linux_write(1, &c, 1);
}

static int linux_console_getchar()
{
	char c;

	if (linux_read(0, &c, 1) < 0) {
		DBG("linux_console read failed (%s)\n", linux_strerror(linux_errno));
		return 0;
	}
	if (c == 0x7f)
		return KEY_BACKSPACE;
	else
		return c;
}

static int linux_console_iskey()
{
	struct pollfd pfd;
	pfd.fd = 0;
	pfd.events = POLLIN;
	if (linux_poll(&pfd, 1, 0) == -1) {
		DBG("linux_console poll failed (%s)\n", linux_strerror(linux_errno));
		return 0;
	}
	if (pfd.revents & POLLIN)
		return 1;
	else
		return 0;
}

struct console_driver linux_console __console_driver = {
	.disabled = 0,
	.putchar = linux_console_putchar,
	.getchar = linux_console_getchar,
	.iskey = linux_console_iskey,
};

int linux_tcgetattr(int fd, struct termios * termios_p)
{
	return linux_ioctl(fd, TCGETS, termios_p);
}

int linux_tcsetattr(int fd, int optional_actions, const struct termios * termios_p)
{
	unsigned long int cmd;

	switch (optional_actions)
	{
		case TCSANOW:
			cmd = TCSETS;
			break;
		case TCSADRAIN:
			cmd = TCSETSW;
			break;
		case TCSAFLUSH:
			cmd = TCSETSF;
			break;
		default:
			linux_errno = EINVAL;
			return -1;
	}

	return linux_ioctl(fd, cmd, termios_p);
}

static struct termios saved_termios;

static void linux_console_startup(void)
{
	struct termios t;

	if (linux_tcgetattr(0, &t)) {
		DBG("linux_console tcgetattr failed (%s)", linux_strerror(linux_errno));
		return;
	}

	saved_termios = t;

	t.c_lflag &= ~(ECHO | ICANON);
	t.c_cc[VINTR] = 0;

	if (linux_tcsetattr(0, TCSAFLUSH, &t))
		DBG("linux_console tcsetattr failed (%s)", linux_strerror(linux_errno));
}

static void linux_console_shutdown(int flags __unused) {
	if (linux_tcsetattr(0, TCSAFLUSH, &saved_termios))
		DBG("linux_console tcsetattr failed (%s)", linux_strerror(linux_errno));
}

struct startup_fn linux_console_startup_fn __startup_fn(STARTUP_EARLY) = {
	.startup = linux_console_startup,
	.shutdown = linux_console_shutdown,
};
