/*
 * Copyright (C) 2008 Michael Brown <mbrown@fensystems.co.uk>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

FILE_LICENCE ( GPL2_OR_LATER );

#include <gpxe/timer.h>

#include <linux_api.h>

/** @file
 *
 * gPXE timer API for linux
 *
 */

/**
 * Delay for a fixed number of microseconds
 *
 * @v usecs		Number of microseconds for which to delay
 */
static void linux_udelay(unsigned long usecs)
{
	linux_usleep(usecs);
}

/**
 * Get number of ticks per second
 *
 * @ret ticks_per_sec	Number of ticks per second
 */
static unsigned long linux_ticks_per_sec(void)
{
	return 1000;
}

/**
 * Get current system time in ticks
 *
 * @ret ticks		Current time, in ticks
 */
static unsigned long linux_currticks(void)
{
	static struct timeval start;
	static int initialized = 0;

	if (! initialized) {
		linux_gettimeofday(&start, NULL);
		initialized = 1;
	}

	struct timeval now;
	linux_gettimeofday(&now, NULL);

	unsigned long ticks = (now.tv_sec - start.tv_sec) * linux_ticks_per_sec();
	ticks += (now.tv_usec - start.tv_usec) / (long)(1000000 / linux_ticks_per_sec());

	return ticks;
}

PROVIDE_TIMER(linux, udelay, linux_udelay);
PROVIDE_TIMER(linux, currticks, linux_currticks);
PROVIDE_TIMER(linux, ticks_per_sec, linux_ticks_per_sec);
