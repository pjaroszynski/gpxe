#include <hci/linux_args.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <gpxe/settings.h>
#include <gpxe/linux.h>
#include <gpxe/malloc.h>
#include <gpxe/init.h>

static int saved_argc = 0;
static char ** saved_argv;

__asmcall void save_args(int argc, char ** argv)
{
	saved_argc = argc;
	saved_argv = argv;
}

static struct option options[] = {
	{"net", 1, 0, 'n'},
	{0, 0, 0, 0}
};

static int parse_net_args(char * args)
{
	char * driver;
	char * token;
	char * name;
	char * value;
	struct linux_device_request * dev_request;
	struct linux_device_request_setting * dev_setting;

	driver = strsep(&args, ",");

	if (strlen(driver) == 0) {
		printf("Missing driver name");
		return -1;
	}

	dev_request = malloc(sizeof(*dev_request));

	dev_request->driver = driver;
	INIT_LIST_HEAD(&dev_request->settings);
	list_add_tail(&dev_request->list, &linux_device_requests);

	while ((token = strsep(&args, ",")) != NULL) {
		name = strsep(&token, "=");
		if (name == NULL)
			continue;
		value = strsep(&token, "=");
		if (value == NULL) {
			DBG("Bad parameter: %s\n", name);
			continue;
		}

		dev_setting = malloc(sizeof(*dev_setting));
		dev_setting->name = name;
		dev_setting->value = value;
		list_add(&dev_setting->list, &dev_request->settings);
	}

	return 0;
}

void linux_args_parse()
{
	int c;
	int rc;

	reset_getopt();
	while (1) {
		int option_index = 0;

		c = getopt_long(saved_argc, saved_argv, "", options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'n':
			if ((rc = parse_net_args(optarg)) != 0)
				return;
			break;
		default:
			return;
		}
	}

	return;
}

void linux_args_cleanup(int flags __unused)
{
	struct linux_device_request * request;
	struct linux_device_request * rtmp;
	struct linux_device_request_setting * setting;
	struct linux_device_request_setting * stmp;

	list_for_each_entry_safe(request, rtmp, &linux_device_requests, list) {
		list_for_each_entry_safe(setting, stmp, &request->settings, list) {
			list_del(&setting->list);
			free(setting);
		}
		list_del(&request->list);
		free(request);
	}
}

struct startup_fn startup_linux_args __startup_fn(STARTUP_EARLY) = {
	.startup = linux_args_parse,
	.shutdown = linux_args_cleanup,
};
