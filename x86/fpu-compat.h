#ifndef KVM_KMOD_FPU_COMPAT_H
#define KVM_KMOD_FPU_COMPAT_H 1

/* Compatibility header for the x86 FPU internal functions.  Replaces
 * asm/fpu/internal.h.
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,2,0)
#include <asm/fpu/internal.h>

static inline void kvm___copy_kernel_to_fpregs(union fpregs_state *fpstate, u64 mask)
{
	if (use_xsave()) {
		copy_kernel_to_xregs(&fpstate->xsave, mask);
	} else {
		if (use_fxsr())
			copy_kernel_to_fxregs(&fpstate->fxsave);
		else
			copy_kernel_to_fregs(&fpstate->fsave);
	}
}

static inline void kvm_copy_kernel_to_fpregs(union fpregs_state *fpstate)
{
	kvm___copy_kernel_to_fpregs(fpstate, -1);
}
#else
#include <asm/i387.h>
#include <asm/fpu-internal.h>
#include <asm/xcr.h>

static inline int kvm___copy_kernel_to_fpregs(struct kvm_compat_fpu *fpu,
					      unsigned long mask)
{
	if (use_xsave()) {
		xrstor_state(&fpu->state->xsave, mask);
		return 0;
	} else
		return fpu_restore_checking(fpu);
}

static inline void kvm_copy_kernel_to_fpregs(struct kvm_compat_fpu *fpu)
{
	kvm___copy_kernel_to_fpregs(fpu, -1);
}
#endif

#endif
