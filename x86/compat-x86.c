#include <linux/types.h>
#include <linux/module.h>

#if IS_ENABLED(CONFIG_HYPERV)

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,17,0)
#include <asm/hyperv-tlfs.h>

struct hv_vp_assist_page **hv_vp_assist_page;
EXPORT_SYMBOL_GPL(hv_vp_assist_page);
#endif
#endif
