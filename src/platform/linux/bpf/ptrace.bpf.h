#ifndef VIGIL_PLATFORM_LINUX_PTRACE_BPF_H
#define VIGIL_PLATFORM_LINUX_PTRACE_BPF_H

#define PTRACE_ATTACH 16
#define PTRACE_SEIZE 16902

SEC("tracepoint/syscalls/sys_enter_ptrace")
int onPtrace(struct trace_event_raw_sys_enter* ctx) {
   const long request = (long)ctx->args[0];
   if (request != PTRACE_ATTACH && request != PTRACE_SEIZE)
      return 0;

   struct PtraceEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_PTRACE);
   e->targetPid = (__u32)ctx->args[1];
   e->request = (__u64)request;

   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif // VIGIL_PLATFORM_LINUX_PTRACE_BPF_H
