#ifndef _LINUX_H
#define _LINUX_H

#include <gpxe/list.h>
#include <gpxe/device.h>

struct linux_device;

struct linux_device_request_setting
{
	char * name;
	char * value;
	struct list_head list;
};

struct linux_device_request
{
	char * driver;
	struct list_head list;
	struct list_head settings;
};

extern struct list_head linux_device_requests;

struct linux_driver {
	char * name;
	int (*probe)(struct linux_device * device, struct linux_device_request * request);
	void (*remove)(struct linux_device * device);
};

/** Linux driver table */
#define LINUX_DRIVERS __table(struct linux_driver, "linux_drivers")

/** Declare a Linux driver */
#define __linux_driver __table_entry(LINUX_DRIVERS, 01)

struct linux_device {
	struct device dev;
	struct linux_driver * driver;
	void * priv;
};

/**
 * Set Linux driver-private data
 *
 * @v device	Linux device
 * @v priv		Private data
 */
static inline void linux_set_drvdata(struct linux_device * device, void * priv) {
	device->priv = priv;
}

/**
 * Get Linux driver-private data
 *
 * @v device	Linux device
 * @ret priv	Private data
 */
static inline void * linux_get_drvdata(struct linux_device * device) {
	return device->priv;
}

#endif /* _LINUX_H */
