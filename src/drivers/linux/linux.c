#include <errno.h>
#include <string.h>
#include <gpxe/linux.h>
#include <gpxe/malloc.h>
#include <gpxe/settings.h>

LIST_HEAD(linux_device_requests);

static int linux_probe(struct root_device * rootdev)
{
	struct linux_device_request * request;
	struct linux_driver * driver;
	struct linux_device * device = NULL;
	int rc;

	list_for_each_entry(request, &linux_device_requests, list) {
		if (! device)
			device = zalloc(sizeof(*device));

		if (! device) {
			return -ENOMEM;
		}

		rc = 1;

		for_each_table_entry(driver, LINUX_DRIVERS) {
			if ((rc = strcmp(driver->name, request->driver)) == 0)
				break;
		}

		if (rc != 0) {
			DBG("Linux driver '%s' not found\n", request->driver);
			continue;
		}

		list_add(&device->dev.siblings, &rootdev->dev.children);
		device->dev.parent = &rootdev->dev;
		INIT_LIST_HEAD(&device->dev.children);

		if (driver->probe(device, request) == 0) {
			device->driver = driver;
			device = NULL;
		} else {
			list_del(&device->dev.siblings);
		}
	};

	free(device);

	return 0;
}

static void linux_remove(struct root_device * rootdev)
{
	struct linux_device * device;
	struct linux_device * tmp;

	list_for_each_entry_safe(device, tmp, &rootdev->dev.children, dev.siblings) {
		list_del(&device->dev.siblings);
		device->driver->remove(device);
		free(device);
	}
}

static struct root_driver linux_root_driver = {
	.probe = linux_probe,
	.remove = linux_remove,
};

struct root_device linux_root_device __root_device = {
	.dev = { .name = "linux" },
	.driver = &linux_root_driver,
};
