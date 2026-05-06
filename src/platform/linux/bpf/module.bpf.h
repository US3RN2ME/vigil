
#ifndef VIGIL_PLATFORM_LINUX_MODULE_BPF_H
#define VIGIL_PLATFORM_LINUX_MODULE_BPF_H

static __always_inline int emitModuleEvent(void) {
   struct ModuleEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;
   fillHeader(&e->hdr, EVENT_MODULE);
   bpf_ringbuf_submit(e, 0);
   return 0;
}

SEC("tracepoint/syscalls/sys_enter_init_module")
int onInitModule(struct trace_event_raw_sys_enter* ctx) {
   return emitModuleEvent();
}

SEC("tracepoint/syscalls/sys_enter_finit_module")
int onFinitModule(struct trace_event_raw_sys_enter* ctx) {
   return emitModuleEvent();
}

#endif // VIGIL_PLATFORM_LINUX_MODULE_BPF_H
