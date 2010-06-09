#ifndef _LINUX_API_PLATFORM_H
#define _LINUX_API_PLATFORM_H

#ifndef PLATFORM_linuxlibc

extern int linux_errno;

#else

extern int * __errno_location(void);
#define linux_errno (*__errno_location ())

#endif

#endif /* _LINUX_API_PLATFORM_H */
