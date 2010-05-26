#ifndef _GPXE_LINUX_UMALLOC_H
#define _GPXE_LINUX_UMALLOC_H

/** @file
 *
 * gPXE user memory allocation API for EFI
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

#ifdef UMALLOC_LINUX
#define UMALLOC_PREFIX_linux
#else
#define UMALLOC_PREFIX_linux __linux_
#endif

#endif /* _GPXE_LINUX_UMALLOC_H */
