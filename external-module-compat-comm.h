
/*
 * Compatibility header for building as an external module.
 */

/*
 * Avoid picking up the kernel's kvm.h in case we have a newer one.
 */

#include <linux/compiler.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/kvm.h>
#include <linux/kvm_para.h>
#include <linux/kconfig.h>
#include <linux/cpu.h>
#include <linux/pci.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/kernel.h>
#include <linux/swait.h>
#include <linux/compat.h>
#include <asm/processor.h>
#include <linux/hrtimer.h>
#include <asm/bitops.h>
#include <linux/kconfig.h>
#include <linux/poll.h>

#include "kvm-kmod-config.h"


#if LINUX_VERSION_CODE < KERNEL_VERSION(4,17,0)
typedef int vm_fault_t;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
#define is_signed_type(type)       (((type)(-1)) < (type)1)
#define __type_half_max(type) ((type)1 << (8*sizeof(type) - 1 - is_signed_type(type)))
#define type_max(T) ((T)((__type_half_max(T) - 1) + __type_half_max(T)))

static inline __must_check size_t array_size(size_t a, size_t b)
{
	size_t bytes = a * b;
        if (__builtin_constant_p(b)
	    ? b > 0 && a > type_max(typeof(a)) / b
	    : a > 0 && b > type_max(typeof(b)) / a)
		return SIZE_MAX;

	return bytes;
}

static inline void *kvcalloc(size_t n, size_t size, gfp_t flags)
{
	return kvmalloc_array(n, size, flags | __GFP_ZERO);
}

static inline int vfs_poll(struct file *file, poll_table *pt)
{
	return file->f_op->poll(file, pt);
}
#endif
