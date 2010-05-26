FILE_LICENCE ( GPL2_OR_LATER );

#include <assert.h>
#include <gpxe/umalloc.h>

#include <linux_api.h>

/** @file
 *
 * gPXE user memory allocation API for Linux
 *
 */

#define NOWHERE ((void *)-1)

#define POISON 0xa5a5a5a5
#define min(a,b) (((a)<(b))?(a):(b))

struct metadata
{
	unsigned poison;
	size_t size;
};

#define SIZE_MD (sizeof(struct metadata))

static void * linux_realloc(void * ptr, size_t size)
{
	struct metadata md = {0, 0};
	struct metadata * mdptr = NULL;

	DBG("linux_realloc(%p, %zd)\n", ptr, size);

	if (ptr != NULL && ptr != NOWHERE) {
		mdptr = ptr - SIZE_MD;
		md = *mdptr;

		if (md.poison != POISON) {
			DBG("linux_realloc bad cookie: 0x%x (expected 0x%x)\n", md.poison, POISON);
			return NULL;
		}
	} else {
		ptr = NULL;
	}

	if (size == 0) {
		if (mdptr) {
			if (linux_munmap(mdptr, md.size))
				DBG("linux_realloc munmap failed: %s\n", linux_strerror(linux_errno));
		}
		return NOWHERE;
	}

	if (ptr) {
		mdptr = linux_mremap(mdptr, md.size + SIZE_MD, size + SIZE_MD, MREMAP_MAYMOVE);
		if (mdptr == MAP_FAILED) {
			DBG("linux_realloc mremap failed: %s\n", linux_strerror(linux_errno));
			return NULL;
		}

		ptr = ((void *)mdptr) + SIZE_MD;
	} else {
		mdptr = linux_mmap(NULL, size + SIZE_MD, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (mdptr == MAP_FAILED) {
			DBG("linux_realloc mmap failed: %s\n", linux_strerror(linux_errno));
			return NULL;
		}
		ptr = ((void *)mdptr) + SIZE_MD;
	}

	mdptr->poison = POISON;
	mdptr->size = size;

	return ptr;
}

/**
 * Reallocate external memory
 *
 * @v old_ptr		Memory previously allocated by umalloc(), or UNULL
 * @v new_size		Requested size
 * @ret new_ptr		Allocated memory, or UNULL
 *
 * Calling realloc() with a new size of zero is a valid way to free a
 * memory block.
 */
static userptr_t linux_urealloc(userptr_t old_ptr, size_t new_size)
{
	return (userptr_t)linux_realloc((void *)old_ptr, new_size);
}

PROVIDE_UMALLOC(linux, urealloc, linux_urealloc);
