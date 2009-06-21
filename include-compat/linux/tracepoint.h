#ifndef _LINUX_TRACEPOINT_H
#define _LINUX_TRACEPOINT_H

struct tracepoint;

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

#define DEFINE_TRACE(name)
#define EXPORT_TRACEPOINT_SYMBOL_GPL(name)
#define EXPORT_TRACEPOINT_SYMBOL(name)

static inline void tracepoint_update_probe_range(struct tracepoint *begin,
	struct tracepoint *end)
{ }

#define PARAMS(args...) args
#define TP_PROTO(args...)	args
#define TP_ARGS(args...)		args

#define TRACE_EVENT(name, proto, args, struct, assign, print)	\
	DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

static inline void tracepoint_synchronize_unregister(void)
{
}

#endif
