
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
