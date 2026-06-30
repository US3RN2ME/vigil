#ifndef VIGIL_PLATFORM_PTRACE_BPF_H
#define VIGIL_PLATFORM_PTRACE_BPF_H

#define PTRACE_ATTACH 16
#define PTRACE_SEIZE 16902

struct PtraceArgs {
   __u32 targetPid;
   __u64 request;
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 1024);
   __type(key, __u32); // tid
   __type(value, struct PtraceArgs);
} ptrace_args SEC(".maps");
// clang-format on

SEC("tracepoint/syscalls/sys_enter_ptrace")
int onPtrace(struct trace_event_raw_sys_enter* ctx) {
   const long request = (long)ctx->args[0];
   if (request != PTRACE_ATTACH && request != PTRACE_SEIZE)
      return 0;

   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   const struct PtraceArgs args = {
       .targetPid = (__u32)ctx->args[1],
       .request = (__u64)request,
   };
   bpf_map_update_elem(&ptrace_args, &tid, &args, BPF_ANY);
   return 0;
}

SEC("tracepoint/syscalls/sys_exit_ptrace")
int onPtraceExit(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct PtraceArgs* args = bpf_map_lookup_elem(&ptrace_args, &tid);
   if (!args)
      return 0;
   const struct PtraceArgs savedArgs = *args;
   bpf_map_delete_elem(&ptrace_args, &tid);

   if (ctx->ret != 0)
      return 0;

   struct PtraceEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_PTRACE);
   e->targetPid = savedArgs.targetPid;
   e->request = savedArgs.request;

   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif // VIGIL_PLATFORM_PTRACE_BPF_H
