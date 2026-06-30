
#ifndef VIGIL_PLATFORM_MODULE_BPF_H
#define VIGIL_PLATFORM_MODULE_BPF_H

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 1024);
   __type(key, __u32); // tid
   __type(value, __u8);
} module_args SEC(".maps");
// clang-format on

static __always_inline int rememberModuleAttempt(void) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   const __u8 present = 1;
   bpf_map_update_elem(&module_args, &tid, &present, BPF_ANY);
   return 0;
}

static __always_inline int emitSuccessfulModuleEvent(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   __u8* present = bpf_map_lookup_elem(&module_args, &tid);
   if (!present)
      return 0;
   bpf_map_delete_elem(&module_args, &tid);

   if (ctx->ret != 0)
      return 0;

   struct ModuleEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;
   fillHeader(&e->hdr, EVENT_MODULE);
   bpf_ringbuf_submit(e, 0);
   return 0;
}

SEC("tracepoint/syscalls/sys_enter_init_module")
int onInitModule(struct trace_event_raw_sys_enter* ctx) {
   return rememberModuleAttempt();
}

SEC("tracepoint/syscalls/sys_exit_init_module")
int onInitModuleExit(struct trace_event_raw_sys_exit* ctx) {
   return emitSuccessfulModuleEvent(ctx);
}

SEC("tracepoint/syscalls/sys_enter_finit_module")
int onFinitModule(struct trace_event_raw_sys_enter* ctx) {
   return rememberModuleAttempt();
}

SEC("tracepoint/syscalls/sys_exit_finit_module")
int onFinitModuleExit(struct trace_event_raw_sys_exit* ctx) {
   return emitSuccessfulModuleEvent(ctx);
}

#endif // VIGIL_PLATFORM_MODULE_BPF_H
