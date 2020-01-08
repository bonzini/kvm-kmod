
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
#include <linux/bitmap.h>

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

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0)
#define prepare_to_swait_exclusive prepare_to_swait
#define swake_up_one               swake_up
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,20,0)
static inline unsigned long *bitmap_alloc(unsigned int nbits, gfp_t flags)
{
	return kmalloc_array(BITS_TO_LONGS(nbits), sizeof(unsigned long), flags);
}

static inline unsigned long *bitmap_zalloc(unsigned int nbits, gfp_t flags)
{
	return bitmap_alloc(nbits, flags | __GFP_ZERO);
}

static inline void bitmap_free(const unsigned long *bitmap)
{
	kfree(bitmap);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
struct mmu_notifier_range {
       struct mm_struct *mm;
       unsigned long start;
       unsigned long end;
       bool blockable;
};

#define kvm_access_ok(a, b) access_ok(NULL, a, b)
#else
#define kvm_access_ok(a, b) access_ok(a, b)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,3,0)
#define ktime_get_boottime_ns() ktime_get_boot_ns()
#define housekeeping_enabled(x) (0)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#define HRTIMER_MODE_ABS_HARD HRTIMER_MODE_ABS
static inline bool cpu_smt_possible(void) { return true; }
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,5,0)
#define rcu_replace_pointer(rcu_ptr, ptr, c)                            	 \
	({                                                                       \
	         typeof(ptr) __tmp = rcu_dereference_protected((rcu_ptr), (c));  \
	         rcu_assign_pointer((rcu_ptr), (ptr));                           \
	         __tmp;                                                          \
	 })
#endif
