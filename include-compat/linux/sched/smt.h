/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_SMT_H
#define _LINUX_SCHED_SMT_H

#ifdef CONFIG_SCHED_SMT
static inline bool sched_smt_active(void) { return true; }
#else
static inline bool sched_smt_active(void) { return false; }
#endif

#endif
