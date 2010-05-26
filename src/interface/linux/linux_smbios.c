FILE_LICENCE ( GPL2_OR_LATER );

#include <errno.h>
#include <gpxe/smbios.h>

/**
 * Find SMBIOS
 *
 * @v smbios		SMBIOS entry point descriptor structure to fill in
 * @ret rc		Return status code
 */
static int linux_find_smbios(struct smbios *smbios __unused)
{
	return -ENODEV;
}

PROVIDE_SMBIOS(linux, find_smbios, linux_find_smbios);
