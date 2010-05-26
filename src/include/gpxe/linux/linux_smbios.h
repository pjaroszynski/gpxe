#ifndef _GPXE_LINUX_SMBIOS_H
#define _GPXE_LINUX_SMBIOS_H

/** @file
 *
 * gPXE SMBIOS API for Linux
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

#ifdef SMBIOS_LINUX
#define SMBIOS_PREFIX_linux
#else
#define SMBIOS_PREFIX_linux __linux_
#endif

#endif /* _GPXE_LINUX_SMBIOS_H */
