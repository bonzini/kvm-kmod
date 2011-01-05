
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)

unsigned int kvm_xstate_size;

void kvm_xstate_size_init(void)
{
	unsigned int eax, ebx, ecx, edx;

	/*  kvm only uses xstate_size if xsave is supported */
	if (cpu_has_xsave) {
		cpuid_count(0xd, 0, &eax, &ebx, &ecx, &edx);
		kvm_xstate_size = ebx;
	}
}

#endif /* < 2.6.36 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)

const int kvm_amd_erratum_383[] =
	AMD_OSVW_ERRATUM(3, AMD_MODEL_RANGE(0x10, 0, 0, 0xff, 0xf));

EXPORT_SYMBOL_GPL(kvm_amd_erratum_383);

#endif /* < 2.6.36 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) && defined(CONFIG_KVM_GUEST)
void kvm_async_pf_task_wait(u32 token)
{
	BUG();
}
EXPORT_SYMBOL_GPL(kvm_async_pf_task_wait);

void kvm_async_pf_task_wake(u32 token)
{
	BUG();
}
EXPORT_SYMBOL_GPL(kvm_async_pf_task_wake);

u32 kvm_read_and_reset_pf_reason(void)
{
	BUG();
	return 0;
}
EXPORT_SYMBOL_GPL(kvm_read_and_reset_pf_reason);
#endif /* < 2.6.38 && CONFIG_KVM_GUEST */
