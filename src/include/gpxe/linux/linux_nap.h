#ifndef _GPXE_LINUX_NAP_H
#define _GPXE_LINUX_NAP_H

/** @file
 *
 * Linux CPU sleeping
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

#ifdef NAP_LINUX
#define NAP_PREFIX_linux
#else
#define NAP_PREFIX_linux __linux_
#endif

#endif /* _GPXE_LINUX_NAP_H */
