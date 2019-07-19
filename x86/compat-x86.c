#include <linux/types.h>
#include <linux/module.h>

#if IS_ENABLED(CONFIG_HYPERV)

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,17,0)
#include <asm/hyperv-tlfs.h>

struct hv_vp_assist_page **hv_vp_assist_page;
EXPORT_SYMBOL_GPL(hv_vp_assist_page);
#endif
#endif

#ifdef KVM_KMOD_NEED_L1TF_DEFS
#include <asm/vmx.h>

enum vmx_l1d_flush_state l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_NEVER;
EXPORT_SYMBOL_GPL(l1tf_vmx_mitigation);
#endif

#ifdef KVM_KMOD_NEED_ITLB_DEFS
int itlb_multihit_kvm_mitigation;
#endif
