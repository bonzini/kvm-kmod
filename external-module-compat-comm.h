
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
#include <linux/cpu.h>
#include <linux/time.h>
#include <asm/processor.h>
#include <linux/hrtimer.h>
#include <asm/bitops.h>

#include "kvm-kmod-config.h"

/*
 * 2.6.16 does not have GFP_NOWAIT
 */

#include <linux/gfp.h>

#ifndef GFP_NOWAIT
#define GFP_NOWAIT (GFP_ATOMIC & ~__GFP_HIGH)
#endif


/*
 * kvm profiling support needs 2.6.20
 */
#include <linux/profile.h>

#ifndef KVM_PROFILING
#define KVM_PROFILING 1234
#define prof_on       4321
#endif

/*
 * smp_call_function_single() is not exported below 2.6.20, and has different
 * semantics below 2.6.23.  The 'nonatomic' argument was removed in 2.6.27.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

int kvm_smp_call_function_single(int cpu, void (*func)(void *info),
				 void *info, int wait);
#undef smp_call_function_single
#define smp_call_function_single kvm_smp_call_function_single

#endif

/* on_each_cpu() lost an argument in 2.6.27. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

#define kvm_on_each_cpu(func, info, wait) on_each_cpu(func, info, 0, wait)

#else

#define kvm_on_each_cpu(func, info, wait) on_each_cpu(func, info, wait)

#endif

#include <linux/notifier.h>
#ifndef CPU_TASKS_FROZEN

#define CPU_TASKS_FROZEN       0x0010
#define CPU_ONLINE_FROZEN      (CPU_ONLINE | CPU_TASKS_FROZEN)
#define CPU_UP_PREPARE_FROZEN  (CPU_UP_PREPARE | CPU_TASKS_FROZEN)
#define CPU_UP_CANCELED_FROZEN (CPU_UP_CANCELED | CPU_TASKS_FROZEN)
#define CPU_DOWN_PREPARE_FROZEN        (CPU_DOWN_PREPARE | CPU_TASKS_FROZEN)
#define CPU_DOWN_FAILED_FROZEN (CPU_DOWN_FAILED | CPU_TASKS_FROZEN)
#define CPU_DEAD_FROZEN                (CPU_DEAD | CPU_TASKS_FROZEN)

#endif

#ifndef CPU_DYING
#define CPU_DYING 0x000A
#define CPU_DYING_FROZEN (CPU_DYING | CPU_TASKS_FROZEN)
#endif

#include <asm/system.h>

struct inode;

#include <linux/fs.h>
#include <linux/anon_inodes.h>

/* anon_inodes on RHEL >= 5.2 is equivalent to 2.6.27 version */
#ifdef RHEL_RELEASE_CODE
#  if (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(5,2)) && defined(CONFIG_ANON_INODES)
#    define RHEL_ANON_INODES
#  endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) && !defined(RHEL_ANON_INODES)

static inline int kvm_anon_inode_getfd(const char *name,
				       const struct file_operations *fops,
				       void *priv, int flags)
{
	int r;
	int fd;
	struct inode *inode;
	struct file *file;

	r = anon_inode_getfd(&fd, &inode, &file, name, fops, priv);
	if (r < 0)
		return r;
	return fd;
}

#elif LINUX_VERSION_CODE == KERNEL_VERSION(2,6,26) && !defined(RHEL_ANON_INODES)

#define kvm_anon_inode_getfd(name, fops, priv, flags) \
	anon_inode_getfd(name, fops, priv)
}

#else /* > 2.6.26 || RHEL_ANON_INODES */

#define kvm_anon_inode_getfd	anon_inode_getfd

#endif /* > 2.6.26 || RHEL_ANON_INODES */

/* div64_u64 is fairly new */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

#define div64_u64 kvm_div64_u64

#ifdef CONFIG_64BIT

static inline uint64_t div64_u64(uint64_t dividend, uint64_t divisor)
{
	return dividend / divisor;
}

#else

uint64_t div64_u64(uint64_t dividend, uint64_t divisor);

#endif

#endif

/*
 * PF_VCPU is a Linux 2.6.24 addition
 */

#include <linux/sched.h>

#ifndef PF_VCPU
#define PF_VCPU 0
#endif

/*
 * smp_call_function_mask() is not defined/exported below 2.6.24 on all
 * targets and below 2.6.26 on x86-64
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) || \
    (defined CONFIG_X86_64 && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))

int kvm_smp_call_function_mask(cpumask_t mask, void (*func) (void *info),
			       void *info, int wait);

#define smp_call_function_mask kvm_smp_call_function_mask

void kvm_smp_send_reschedule(int cpu);

#else

#define kvm_smp_send_reschedule smp_send_reschedule

#endif

/* empty_zero_page isn't exported in all kernels */
#include <asm/pgtable.h>

#define empty_zero_page kvm_empty_zero_page

static char empty_zero_page[PAGE_SIZE];

static inline void blahblah(void)
{
	(void)empty_zero_page[0];
}

/* __mmdrop() is not exported before 2.6.25 */
#include <linux/sched.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)

#define mmdrop(x) do { (void)(x); } while (0)
#define mmget(x) do { (void)(x); } while (0)

#else

#define mmget(x) do { atomic_inc(x); } while (0)

#endif

#ifdef KVM_NEED_PAGEFAULT_DISABLE

static inline void pagefault_disable(void)
{
	inc_preempt_count();
	/*
	 * make sure to have issued the store before a pagefault
	 * can hit.
	 */
	barrier();
}

static inline void pagefault_enable(void)
{
	/*
	 * make sure to issue those last loads/stores before enabling
	 * the pagefault handler again.
	 */
	barrier();
	dec_preempt_count();
	/*
	 * make sure we do..
	 */
	barrier();
	preempt_check_resched();
}

#endif

/* simple vfs attribute getter signature has changed to add a return code */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)

#define MAKE_SIMPLE_ATTRIBUTE_GETTER(x)       \
	static u64 x(void *v)                 \
	{				      \
		u64 ret = 0;		      \
					      \
		__##x(v, &ret);		      \
		return ret;		      \
	}

#else

#define MAKE_SIMPLE_ATTRIBUTE_GETTER(x)       \
	static int x(void *v, u64 *val)	      \
	{				      \
		return __##x(v, val);	      \
	}

#endif

/* set_kset_name() is gone in 2.6.25 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

#define set_kset_name(x) .name = x

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
#ifndef FASTCALL
#define FASTCALL(x)	x
#define fastcall
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

static inline void flush_work(struct work_struct *work)
{
	cancel_work_sync(work);
}

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)

#include <linux/mm.h>

static inline int kvm___get_user_pages_fast(unsigned long start, int nr_pages,
					    int write, struct page **pages)
{
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

static inline int get_user_pages_fast(unsigned long start, int nr_pages,
				      int write, struct page **pages)
{
	int npages;

	down_read(&current->mm->mmap_sem);
	npages = get_user_pages(current, current->mm, start, nr_pages, write,
				0, pages, NULL);
	up_read(&current->mm->mmap_sem);

	return npages;
}

#endif /* < 2.6.27 */

#else /* >= 2.6.37 */

#define kvm___get_user_pages_fast	__get_user_pages_fast

#endif /* >= 2.6.37 */

/* spin_needbreak() was called something else in 2.6.24 */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,24)

#define spin_needbreak need_lockbreak

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)

static inline void kvm_hrtimer_add_expires_ns(struct hrtimer *timer, u64 delta)
{
	timer->expires = ktime_add_ns(timer->expires, delta);
}

static inline ktime_t kvm_hrtimer_get_expires(struct hrtimer *timer)
{
	return timer->expires;
}

static inline u64 kvm_hrtimer_get_expires_ns(struct hrtimer *timer)
{
	return ktime_to_ns(timer->expires);
}

static inline void kvm_hrtimer_start_expires(struct hrtimer *timer, int mode)
{
	hrtimer_start(timer, timer->expires, mode);
}

static inline ktime_t kvm_hrtimer_expires_remaining(const struct hrtimer *timer)
{
    return ktime_sub(timer->expires, timer->base->get_time());
}

#else

#define kvm_hrtimer_add_expires_ns hrtimer_add_expires_ns
#define kvm_hrtimer_get_expires hrtimer_get_expires
#define kvm_hrtimer_get_expires_ns hrtimer_get_expires_ns
#define kvm_hrtimer_start_expires hrtimer_start_expires
#define kvm_hrtimer_expires_remaining hrtimer_expires_remaining

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
#include <linux/pci.h>

static inline int __pci_reset_function(struct pci_dev *dev)
{
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
static inline int pci_reset_function(struct pci_dev *dev)
{
	return 0;
}
#endif /* < 2.6.28 */
#endif /* < 2.6.31 */

/* dynamically allocated cpu masks introduced in 2.6.28 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)

typedef cpumask_t cpumask_var_t[1];

static inline bool alloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	return 1;
}

static inline void free_cpumask_var(cpumask_var_t mask)
{
}

static inline void cpumask_clear(cpumask_var_t mask)
{
	cpus_clear(*mask);
}

static inline void cpumask_set_cpu(int cpu, cpumask_var_t mask)
{
	cpu_set(cpu, *mask);
}

static inline int smp_call_function_many(cpumask_var_t cpus,
					 void (*func)(void *data), void *data,
					 int sync)
{
	return smp_call_function_mask(*cpus, func, data, sync);
}

static inline int cpumask_empty(cpumask_var_t mask)
{
	return cpus_empty(*mask);
}

static inline int cpumask_test_cpu(int cpu, cpumask_var_t mask)
{
	return cpu_isset(cpu, *mask);
}

static inline void cpumask_clear_cpu(int cpu, cpumask_var_t mask)
{
	cpu_clear(cpu, *mask);
}

#define cpu_online_mask (&cpu_online_map)

#define cpumask_any(m) first_cpu(*(m))

#endif

/* A zeroing constructor was added late 2.6.30 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)

static inline bool zalloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	bool ret;

	ret = alloc_cpumask_var(mask, flags);
	if (ret)
		cpumask_clear(*mask);
	return ret;
}

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)

#define IF_ANON_INODES_DOES_REFCOUNTS(x)

#else

#define IF_ANON_INODES_DOES_REFCOUNTS(x) x

#endif


/* Macro introduced only on newer kernels: */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
#define marker_synchronize_unregister() synchronize_sched()
#endif

#ifdef NEED_COMPOUND_HEAD

static inline struct page *compound_head(struct page *page)
{
	if (PageCompound(page))
		page = (struct page *)page_private(page);
	return page;
}

#endif

#include <linux/iommu.h>
#ifndef IOMMU_CACHE

#define IOMMU_CACHE	(4)
#define IOMMU_CAP_CACHE_COHERENCY	0x1
static inline int iommu_domain_has_cap(struct iommu_domain *domain,
				       unsigned long cap)
{
	return 0;
}

#endif

/*
 * Tracepoints were introduced in 2.6.28, but changed several times in
 * incompatible ways.
 */

#include <linux/tracepoint.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)

struct tracepoint;

#undef DECLARE_TRACE
#undef DEFINE_TRACE
#undef PARAMS
#undef TP_PROTO
#undef TP_ARGS
#undef EXPORT_TRACEPOINT_SYMBOL
#undef EXPORT_TRACEPOINT_SYMBOL_GPL

#define DECLARE_TRACE(name, proto, args)				\
	static inline void _do_trace_##name(struct tracepoint *tp, proto) \
	{ }								\
	static inline void trace_##name(proto)				\
	{ }								\
	static inline int register_trace_##name(void (*probe)(proto))	\
	{								\
		return -ENOSYS;						\
	}								\
	static inline int unregister_trace_##name(void (*probe)(proto))	\
	{								\
		return -ENOSYS;						\
	}

#define tracepoint_update_probe_range(begin, end) do {} while (0)

#define DEFINE_TRACE(name)
#define EXPORT_TRACEPOINT_SYMBOL_GPL(name)
#define EXPORT_TRACEPOINT_SYMBOL(name)

#define PARAMS(args...) args
#define TP_PROTO(args...)	args
#define TP_ARGS(args...)		args

#define TRACE_EVENT(name, proto, args, struct, assign, print)	\
	DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

#undef tracepoint_synchronize_unregister
#define tracepoint_synchronize_unregister() do {} while (0)

#define DECLARE_EVENT_CLASS(name, proto, args, tstruct, assign, print)
#define DEFINE_EVENT(template, name, proto, args)		\
	DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

#endif

#include <linux/ftrace_event.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)

struct trace_print_flags {
	unsigned long		mask;
	const char		*name;
};

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)

#define alloc_pages_exact_node alloc_pages_node

#endif

#include <linux/hugetlb.h>

/* vma_kernel_pagesize, exported since 2.6.32 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)

#if defined(CONFIG_HUGETLB_PAGE) && LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
static inline
unsigned long kvm_vma_kernel_pagesize(struct vm_area_struct *vma)
{
	struct hstate *hstate;

	if (!is_vm_hugetlb_page(vma))
		return PAGE_SIZE;

	hstate = hstate_vma(vma);

	return 1UL << (hstate->order + PAGE_SHIFT);
}
#else /* !CONFIG_HUGETLB_SIZE || <= 2.6.26 */
#define kvm_vma_kernel_pagesize(v) PAGE_SIZE
#endif

#else /* >= 2.6.32 */

#define kvm_vma_kernel_pagesize vma_kernel_pagesize

#endif

#ifndef printk_once
/*
 * Print a one-time message (analogous to WARN_ONCE() et al):
 */
#define printk_once(x...) ({			\
	static int __print_once = 1;		\
						\
	if (__print_once) {			\
		__print_once = 0;		\
		printk(x);			\
	}					\
})
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) && !defined(CONFIG_CPU_FREQ)
static inline unsigned int cpufreq_get(unsigned int cpu)
{
	return 0;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
int schedule_hrtimeout(ktime_t *expires, const enum hrtimer_mode mode);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#ifndef CONFIG_MMU_NOTIFIER
struct mmu_notifier {};
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
static inline void hlist_del_init_rcu(struct hlist_node *n)
{
	if (!hlist_unhashed(n)) {
		__hlist_del(n);
		n->pprev = NULL;
	}
}
#endif

#ifndef CONFIG_USER_RETURN_NOTIFIER

#include <linux/percpu.h>

struct kvm_user_return_notifier {
	void (*on_user_return)(struct kvm_user_return_notifier *urn);
};

DECLARE_PER_CPU(struct kvm_user_return_notifier *, kvm_urn);

static inline void
kvm_user_return_notifier_register(struct kvm_user_return_notifier *urn)
{
	__get_cpu_var(kvm_urn) = urn;
}

static inline void
kvm_user_return_notifier_unregister(struct kvm_user_return_notifier *urn)
{
	__get_cpu_var(kvm_urn) = NULL;
}

static inline void kvm_fire_urn(void)
{
	struct kvm_user_return_notifier *urn = __get_cpu_var(kvm_urn);

	if (urn)
		urn->on_user_return(urn);
}

#else /* CONFIG_USER_RETURN_NOTIFIER */

#define kvm_user_return_notifier		user_return_notifier
#define kvm_user_return_notifier_register	user_return_notifier_register
#define kvm_user_return_notifier_unregister	user_return_notifier_unregister

static inline void kvm_fire_urn(void) {}

#endif /* CONFIG_USER_RETURN_NOTIFIER */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)

#ifdef CONFIG_SMP
void kvm_synchronize_srcu_expedited(struct srcu_struct *sp);
#else
static inline void kvm_synchronize_srcu_expedited(struct srcu_struct *sp) { }
#endif

#else

#define kvm_synchronize_srcu_expedited synchronize_srcu_expedited

#endif

int kvm_init_srcu(void);
void kvm_exit_srcu(void);

#ifndef WARN_ONCE
#define WARN_ONCE(condition, format...)	({			\
	static bool __warned;					\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN_ON(!__warned)) 			\
			__warned = true;			\
	unlikely(__ret_warn_once);				\
})
#endif

#ifndef WARN
#define WARN(condition, format...)	WARN_ON(condition)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
#define get_online_cpus lock_cpu_hotplug
#define put_online_cpus unlock_cpu_hotplug
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) || \
    (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,32) && KERNEL_EXTRAVERSION < 9)
static inline void kvm_getboottime(struct timespec *ts)
{
	struct timespec sys, now = current_kernel_time();
	ktime_get_ts(&sys);
	*ts = ns_to_timespec(timespec_to_ns(&now) - timespec_to_ns(&sys));
}
#define kvm_monotonic_to_bootbased(ts)
#else
#define kvm_getboottime			getboottime
#define kvm_monotonic_to_bootbased	monotonic_to_bootbased
#endif

static inline void kvm_clock_warn_suspend_bug(void)
{
#if defined(CONFIG_SUSPEND) && \
    (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) || \
     (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,32) && KERNEL_EXTRAVERSION < 9))
	printk("kvm: paravirtual wallclock will not work reliably "
	       "accross host suspend/resume\n");
#endif
}

#if defined(CONFIG_PCI) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33) && \
    (!defined(CONFIG_SUSE_KERNEL) || LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32))
#include <linux/pci.h>
static inline struct pci_dev *
pci_get_domain_bus_and_slot(int domain, unsigned int bus, unsigned int devfn)
{
	if (domain != 0)
		return NULL;
	return pci_get_bus_and_slot(bus, devfn);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)

#ifdef CONFIG_PREEMPT_RT
#if LINUX_VERSION_CODE != KERNEL_VERSION(2,6,31)
#error PREEMPT_RT only supported on 2.6.31
#endif
#define raw_spinlock_t		atomic_spinlock_t
#define raw_spin_lock_init	atomic_spin_lock_init
#define raw_spin_lock		atomic_spin_lock
#define raw_spin_unlock		atomic_spin_unlock

#else

#define raw_spinlock_t		spinlock_t
#define raw_spin_lock_init	spin_lock_init
#define raw_spin_lock		spin_lock
#define raw_spin_unlock		spin_unlock
#endif

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
struct perf_guest_info_callbacks {
	int (*is_in_guest) (void);
	int (*is_user_mode) (void);
	unsigned long (*get_guest_ip) (void);
};

static inline int
perf_register_guest_info_callbacks(struct perf_guest_info_callbacks *cbs)
{
	return 0;
}

static inline int
perf_unregister_guest_info_callbacks(struct perf_guest_info_callbacks *cbs)
{
	return 0;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
#define rcu_dereference_check(p, sp)	rcu_dereference(p)
#define rcu_dereference_protected(p, c)	rcu_dereference(p)
#define srcu_dereference(p, sp)		rcu_dereference(p)
#define srcu_read_lock_held(sp)		(1)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#define lockdep_is_held(m)		(1)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35) && defined(CONFIG_IOMMU_API)
#include <linux/iommu.h>

static inline int iommu_map(struct iommu_domain *domain, unsigned long iova,
			    phys_addr_t paddr, int gfp_order, int prot)
{
	size_t size = 0x1000UL << gfp_order;

	return iommu_map_range(domain, iova, paddr, size, prot);
}

static inline int iommu_unmap(struct iommu_domain *domain, unsigned long iova,
			      int gfp_order)
{
	size_t size = 0x1000UL << gfp_order;

	iommu_unmap_range(domain, iova, size);

	return gfp_order;
}
#endif

#ifndef lower_32_bits
#define lower_32_bits(n) ((u32)(n))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static inline int is_hwpoison_address(unsigned long addr)
{
	return 0;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#include <asm/siginfo.h>

typedef struct {
	int si_signo;
	int si_errno;
	int si_code;

	union {
		int _pad[SI_PAD_SIZE];

		struct _sigfault {
			void __user *_addr; /* faulting insn/memory ref. */
#ifdef __ARCH_SI_TRAPNO
			int _trapno;	/* TRAP # which caused the signal */
#endif
			short _addr_lsb; /* LSB of the reported address */
		} _sigfault;
	} _sifields;
} kvm_siginfo_t;

#define si_addr_lsb	_sifields._sigfault._addr_lsb
#define BUS_MCEERR_AR	(__SI_FAULT|4)

#else

#define kvm_siginfo_t	siginfo_t

#endif

#include <linux/mm.h>

/* Services below are only referenced by code unused in older kernels */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
static inline void kvm_use_mm(struct mm_struct *mm)
{
	BUG();
}

static inline void kvm_unuse_mm(struct mm_struct *mm)
{
	BUG();
}
#else
#define kvm_use_mm	use_mm
#define kvm_unuse_mm	unuse_mm
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
static inline u32 hash_32(u32 val, unsigned int bits)
{
	BUG();
	return 0;
}
#define order_base_2(n)	({ BUG(); 0; })
#endif

#ifndef __rcu
#define __rcu
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
#include <linux/vmalloc.h>
static inline void *vzalloc(unsigned long size)
{
	void *addr = vmalloc(size);
	if (addr)
		memset(addr, 0, size);
	return addr;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#include <linux/interrupt.h>

#define IRQF_ONESHOT	0x00002000

static inline int
kvm_request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn,
                         unsigned long flags, const char *name, void *dev)
{
	return -ENOSYS;
}
#else
#define kvm_request_threaded_irq	request_threaded_irq
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#define compound_trans_head(page) compound_head(page)

static inline int PageTransCompound(struct page *page)
{
        return 0;
}
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,33)
#define kvm___this_cpu_read(n)		__get_cpu_var(n)
#define kvm___this_cpu_write(n, v)	__get_cpu_var(n) = v
#else /* > 2.6.33 */
#define kvm___this_cpu_read		__this_cpu_read
#define kvm___this_cpu_write		__this_cpu_write
#endif /* > 2.6.33 */
