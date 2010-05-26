#ifndef _GPXE_LINUX_UACCESS_H
#define _GPXE_LINUX_UACCESS_H

/** @file
 *
 * gPXE user access API for Linux
 *
 * Linux runs with flat physical addressing, so the various mappings
 * between virtual addresses, I/O addresses and bus addresses are all
 * no-ops.
 */

FILE_LICENCE ( GPL2_OR_LATER );

#ifdef UACCESS_LINUX
#define UACCESS_PREFIX_linux
#else
#define UACCESS_PREFIX_linux __linux_
#endif

/**
 * Convert physical address to user pointer
 *
 * @v phys_addr		Physical address
 * @ret userptr		User pointer
 */
static inline __always_inline userptr_t
UACCESS_INLINE(linux, phys_to_user)(unsigned long phys_addr)
{
	return phys_addr;
}

/**
 * Convert user buffer to physical address
 *
 * @v userptr		User pointer
 * @v offset		Offset from user pointer
 * @ret phys_addr	Physical address
 */
static inline __always_inline unsigned long
UACCESS_INLINE(linux, user_to_phys)(userptr_t userptr, off_t offset)
{
	return userptr + offset;
}

static inline __always_inline userptr_t
UACCESS_INLINE(linux, virt_to_user)(volatile const void * addr)
{
	return trivial_virt_to_user(addr);
}

static inline __always_inline void *
UACCESS_INLINE(linux, user_to_virt)(userptr_t userptr, off_t offset)
{
	return trivial_user_to_virt(userptr, offset);
}

static inline __always_inline userptr_t
UACCESS_INLINE(linux, userptr_add)(userptr_t userptr, off_t offset)
{
	return trivial_userptr_add(userptr, offset);
}

static inline __always_inline void
UACCESS_INLINE(linux, memcpy_user)(userptr_t dest, off_t dest_off, userptr_t src, off_t src_off, size_t len)
{
	trivial_memcpy_user(dest, dest_off, src, src_off, len);
}

static inline __always_inline void
UACCESS_INLINE(linux, memmove_user)(userptr_t dest, off_t dest_off, userptr_t src, off_t src_off, size_t len)
{
	trivial_memmove_user(dest, dest_off, src, src_off, len);
}

static inline __always_inline void
UACCESS_INLINE(linux, memset_user)(userptr_t buffer, off_t offset, int c, size_t len)
{
	trivial_memset_user(buffer, offset, c, len);
}

static inline __always_inline size_t
UACCESS_INLINE ( linux, strlen_user ) ( userptr_t buffer, off_t offset ) {
	return trivial_strlen_user ( buffer, offset );
}

static inline __always_inline off_t
UACCESS_INLINE(linux, memchr_user)(userptr_t buffer, off_t offset, int c, size_t len)
{
	return trivial_memchr_user(buffer, offset, c, len);
}

#endif /* _GPXE_LINUX_UACCESS_H */
