
/*
 * Compatibility header for building as an external module.
 */

#include <linux/compiler.h>
#include <linux/version.h>
#include <linux/kconfig.h>

#include <linux/types.h>

#include <asm/cpufeature.h>

#include "../external-module-compat-comm.h"

#include <asm/msr.h>
#include <asm/msr-index.h>
#include <asm/asm.h>
#include <asm/desc.h>

#ifndef CONFIG_HAVE_KVM_EVENTFD
#define CONFIG_HAVE_KVM_EVENTFD 1
#endif

#ifndef CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT
#define CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT 1
#endif

#ifndef CONFIG_KVM_APIC_ARCHITECTURE
#define CONFIG_KVM_APIC_ARCHITECTURE
#endif

#ifndef CONFIG_KVM_MMIO
#define CONFIG_KVM_MMIO
#endif

#ifndef CONFIG_KVM_ASYNC_PF
#define CONFIG_KVM_ASYNC_PF 1
#endif

#ifndef CONFIG_HAVE_KVM_MSI
#define CONFIG_HAVE_KVM_MSI 1
#endif

#ifndef CONFIG_HAVE_KVM_IRQCHIP
#define CONFIG_HAVE_KVM_IRQCHIP 1
#endif

#ifndef CONFIG_HAVE_KVM_IRQ_ROUTING
#define CONFIG_HAVE_KVM_IRQ_ROUTING 1
#endif

#ifndef CONFIG_HAVE_KVM_IRQFD
#define CONFIG_HAVE_KVM_IRQFD 1
#endif

#ifndef CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT
#define CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT 1
#endif

#include <linux/smp.h>

#ifndef MSR_F15H_PERF_CTL
#define MSR_F15H_PERF_CTL               0xc0010200
#endif
#ifndef MSR_F15H_PERF_CTL0
#define MSR_F15H_PERF_CTL0              MSR_F15H_PERF_CTL
#endif
#ifndef MSR_F15H_PERF_CTL1
#define MSR_F15H_PERF_CTL1              (MSR_F15H_PERF_CTL + 2)
#endif
#ifndef MSR_F15H_PERF_CTL2
#define MSR_F15H_PERF_CTL2              (MSR_F15H_PERF_CTL + 4)
#endif
#ifndef MSR_F15H_PERF_CTL3
#define MSR_F15H_PERF_CTL3              (MSR_F15H_PERF_CTL + 6)
#endif
#ifndef MSR_F15H_PERF_CTL4
#define MSR_F15H_PERF_CTL4              (MSR_F15H_PERF_CTL + 8)
#endif
#ifndef MSR_F15H_PERF_CTL5
#define MSR_F15H_PERF_CTL5              (MSR_F15H_PERF_CTL + 10)
#endif

#ifndef MSR_F15H_PERF_CTR
#define MSR_F15H_PERF_CTR               0xc0010201
#endif
#ifndef MSR_F15H_PERF_CTR0
#define MSR_F15H_PERF_CTR0              MSR_F15H_PERF_CTR
#endif
#ifndef MSR_F15H_PERF_CTR1
#define MSR_F15H_PERF_CTR1              (MSR_F15H_PERF_CTR + 2)
#endif
#ifndef MSR_F15H_PERF_CTR2
#define MSR_F15H_PERF_CTR2              (MSR_F15H_PERF_CTR + 4)
#endif
#ifndef MSR_F15H_PERF_CTR3
#define MSR_F15H_PERF_CTR3              (MSR_F15H_PERF_CTR + 6)
#endif
#ifndef MSR_F15H_PERF_CTR4
#define MSR_F15H_PERF_CTR4              (MSR_F15H_PERF_CTR + 8)
#endif
#ifndef MSR_F15H_PERF_CTR5
#define MSR_F15H_PERF_CTR5              (MSR_F15H_PERF_CTR + 10)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,17,0)
static inline unsigned long cpu_kernelmode_gs_base(int cpu)
{
	unsigned long a;
	rdmsrl(MSR_GS_BASE, a);
	return a;
}

static inline void save_fsgs_for_kvm(void)
{
        savesegment(fs, current->thread.fsindex);
        savesegment(gs, current->thread.gsindex);
        if (unlikely(current->thread.fsindex != 0))
		current->thread.fsbase = 0;
        if (unlikely(current->thread.gsindex != 0))
		current->thread.gsbase = 0;
}
#endif
