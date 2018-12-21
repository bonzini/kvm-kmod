#include <asm/hyperv-tlfs.h>
#include <asm/mshyperv.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,21,0)
int hyperv_flush_guest_mapping_range(u64 as,
		hyperv_fill_flush_list_func fill_func, void *data)
{
	WARN_ON(1);
       	return -ENOTSUPP;
}

int hyperv_fill_flush_guest_mapping_list(
		struct hv_guest_mapping_flush_list *flush,
		u64 start_gfn, u64 pages)
{
	WARN_ON(1);
	return 0;
}
#endif
