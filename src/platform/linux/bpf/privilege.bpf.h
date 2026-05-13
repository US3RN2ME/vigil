
#ifndef VIGIL_PLATFORM_PRIVILEGE_BPF_H
#define VIGIL_PLATFORM_PRIVILEGE_BPF_H

static __always_inline int emitSetuidEvent(__u32 newUid) {
   const __u32 uid = (__u32)bpf_get_current_uid_gid();
   if (uid == 0)
      return 0;

   struct SetuidEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_SETUID);
   e->newUid = newUid;

   bpf_ringbuf_submit(e, 0);
   return 0;
}

SEC("tracepoint/syscalls/sys_enter_setuid")
int onSetuid(struct trace_event_raw_sys_enter* ctx) {
   const __u32 newUid = (__u32)ctx->args[0];
   if (newUid != 0)
      return 0;
   return emitSetuidEvent(newUid);
}

SEC("tracepoint/syscalls/sys_enter_setresuid")
int onSetresuid(struct trace_event_raw_sys_enter* ctx) {
   const __u32 newEuid = (__u32)ctx->args[1];
   if (newEuid != 0)
      return 0;
   return emitSetuidEvent(newEuid);
}

#endif // VIGIL_PLATFORM_PRIVILEGE_BPF_H
