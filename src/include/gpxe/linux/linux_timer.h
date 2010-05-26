#ifndef _GPXE_LINUX_TIMER_H
#define _GPXE_LINUX_TIMER_H

/** @file
 *
 * gPXE timer API for Linux
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

#ifdef TIMER_LINUX
#define TIMER_PREFIX_linux
#else
#define TIMER_PREFIX_linux __linux_
#endif

#endif /* _GPXE_LINUX_TIMER_H */
