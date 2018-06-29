/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SPECCTRL_H_
#define _ASM_X86_SPECCTRL_H_

#include <linux/thread_info.h>
#include <asm/nospec-branch.h>

/**
 * x86_spec_ctrl_set_guest - Set speculation control registers for the guest
 * @guest_spec_ctrl:		The guest content of MSR_SPEC_CTRL
 * @guest_virt_spec_ctrl:	The guest controlled bits of MSR_VIRT_SPEC_CTRL
 *				(may get translated to MSR_AMD64_LS_CFG bits)
 *
 * Avoids writing to the MSR if the content/bits are the same
 */
static inline
void x86_spec_ctrl_set_guest(u64 guest_spec_ctrl, u64 guest_virt_spec_ctrl)
{
	x86_virt_spec_ctrl(guest_spec_ctrl, guest_virt_spec_ctrl, true);
}

/**
 * x86_spec_ctrl_restore_host - Restore host speculation control registers
 * @guest_spec_ctrl:		The guest content of MSR_SPEC_CTRL
 * @guest_virt_spec_ctrl:	The guest controlled bits of MSR_VIRT_SPEC_CTRL
 *				(may get translated to MSR_AMD64_LS_CFG bits)
 *
 * Avoids writing to the MSR if the content/bits are the same
 */
static inline
void x86_spec_ctrl_restore_host(u64 guest_spec_ctrl, u64 guest_virt_spec_ctrl)
{
	x86_virt_spec_ctrl(guest_spec_ctrl, guest_virt_spec_ctrl, false);
}

#endif
