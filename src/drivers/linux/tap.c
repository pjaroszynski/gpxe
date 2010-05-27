#include <errno.h>
#include <string.h>
#include <linux_api.h>
#include <gpxe/list.h>
#include <gpxe/linux.h>
#include <gpxe/malloc.h>
#include <gpxe/device.h>
#include <gpxe/netdevice.h>
#include <gpxe/iobuf.h>
#include <gpxe/ethernet.h>
#include <gpxe/settings.h>
#include <gpxe/socket.h>

#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>

#define RX_BUF_SIZE		1536

struct tap_nic {
	char * interface;
	int fd;
};

static int tap_open(struct net_device * netdev)
{
	struct tap_nic * nic = netdev->priv;
	struct ifreq ifr;
	int ret;

	nic->fd = linux_open("/dev/net/tun", O_RDWR);
	if (nic->fd < 0) {
		DBGC(nic, "tap %p open('/dev/net/tun') = %d (%s)\n", nic, nic->fd, linux_strerror(linux_errno));
		return nic->fd;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	strncpy(ifr.ifr_name, nic->interface, IFNAMSIZ);
	DBGC(nic, "tap %p interface = '%s'\n", nic, nic->interface);

	ret = linux_ioctl(nic->fd, TUNSETIFF, &ifr);

	if (ret != 0) {
		DBGC(nic, "tap %p ioctl(%d, ...) = %d (%s)\n", nic, nic->fd, ret, linux_strerror(linux_errno));
		linux_close(nic->fd);
		return ret;
	}

	ret = linux_fcntl(nic->fd, F_SETFL, O_NONBLOCK);

	if (ret != 0) {
		DBGC(nic, "tap %p fcntl(%d, ...) = %d (%s)\n", nic, nic->fd, ret, linux_strerror(linux_errno));
		linux_close(nic->fd);
		return ret;
	}

	return 0;
}

static void tap_close(struct net_device * netdev)
{
	struct tap_nic * nic = netdev->priv;
	linux_close(nic->fd);
}

static int tap_transmit(struct net_device * netdev, struct io_buffer * iobuf)
{
	struct tap_nic * nic = netdev->priv;
	int rc;

	/* Pad and align packet */
	iob_pad(iobuf, ETH_ZLEN);

	rc = linux_write(nic->fd, iobuf->data, iobuf->tail - iobuf->data);
	DBGC(nic, "tap %p wrote %d bytes\n", nic, rc);
	netdev_tx_complete(netdev, iobuf);

	return 0;
}

static void tap_poll(struct net_device * netdev)
{
	struct tap_nic * nic = netdev->priv;
	struct pollfd pfd;
	struct io_buffer * iobuf;
	int r;

	pfd.fd = nic->fd;
	pfd.events = POLLIN;
	if (linux_poll(&pfd, 1, 0) == -1) {
		DBGC(nic, "tap %p poll failed (%s)\n", nic, linux_strerror(linux_errno));
		return;
	}
	if ((pfd.revents & POLLIN) == 0)
		return;

	iobuf = alloc_iob(RX_BUF_SIZE);
	if (! iobuf)
		goto allocfail;

	while ((r = linux_read(nic->fd, iobuf->data, RX_BUF_SIZE)) > 0) {
		DBGC(nic, "tap %p read %d bytes\n", nic, r);

		iob_put(iobuf, r);
		netdev_rx(netdev, iobuf);

		iobuf = alloc_iob(RX_BUF_SIZE);
		if (! iobuf)
			goto allocfail;
	}

	free_iob(iobuf);
	return;

allocfail:
	DBGC(nic, "tap %p alloc_iob failed\n", nic);
}

static void tap_irq(struct net_device * netdev, int enable)
{
	struct tap_nic * nic = netdev->priv;

	DBGC(nic, "tap %p irq enable = %d\n", nic, enable);
}

static struct net_device_operations tap_operations = {
	.open		= tap_open,
	.close		= tap_close,
	.transmit	= tap_transmit,
	.poll		= tap_poll,
	.irq		= tap_irq,
};

static int tap_probe(struct linux_device * device, struct linux_device_request * request)
{
	struct linux_device_request_setting * setting;
	struct net_device * netdev;
	struct tap_nic * nic;
	int rc;

	netdev = alloc_etherdev(sizeof(*nic));
	if (! netdev)
		return -ENOMEM;

	netdev_init(netdev, &tap_operations);
	nic = netdev->priv;
	linux_set_drvdata(device, netdev);
	netdev->dev = &device->dev;
	memset(nic, 0, sizeof(*nic));

	netdev_link_up(netdev);

	if ((rc = register_netdev(netdev)) != 0)
		goto err_register;

	list_for_each_entry(setting, &request->settings, list) {
		if (strcmp(setting->name, "if") == 0) {
			nic->interface = setting->value;
		} else {
			struct setting * s = find_setting(setting->name);
			if (s) {
				rc = storef_setting(&netdev->settings.settings, find_setting(setting->name), setting->value);
				if (rc != 0)
					DBGC(nic, "tap %p storing setting '%s' = '%s' failed\n", nic, setting->name, setting->value);
			} else {
				DBGC(nic, "tap %p unknown setting '%s'\n", nic, setting->name);
			}
		}
	}

	if (! nic->interface) {
		rc = -EINVAL;
		goto err_settings;
	}

	return 0;

err_settings:
	unregister_netdev(netdev);
err_register:
	netdev_nullify(netdev);
	netdev_put(netdev);
	return rc;
}

static void tap_remove(struct linux_device * device)
{
	struct net_device * netdev = linux_get_drvdata(device);
	unregister_netdev(netdev);
	netdev_nullify(netdev);
	netdev_put(netdev);
}

struct linux_driver tap_driver __linux_driver = {
	.name = "tap",
	.probe = tap_probe,
	.remove = tap_remove
};
