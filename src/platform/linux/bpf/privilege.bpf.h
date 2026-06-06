
#ifndef VIGIL_PLATFORM_PRIVILEGE_BPF_H
#define VIGIL_PLATFORM_PRIVILEGE_BPF_H

struct SetuidArgs {
  __u32 newUid;
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 1024);
   __type(key, __u32); // tid
   __type(value, struct SetuidArgs);
} setuid_args SEC(".maps");
// clang-format on

static __always_inline int emitSetuidEvent(__u32 newUid) {
  struct SetuidEvent *e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
  if (!e)
    return 0;

  fillHeader(&e->hdr, EVENT_SETUID);
  e->newUid = newUid;

  bpf_ringbuf_submit(e, 0);
  return 0;
}

static __always_inline int rememberSetuid(__u32 newUid) {
  const __u32 uid = (__u32)bpf_get_current_uid_gid();
  if (uid == 0 || newUid != 0)
    return 0;

  const __u32 tid = (__u32)bpf_get_current_pid_tgid();
  const struct SetuidArgs args = {.newUid = newUid};
  bpf_map_update_elem(&setuid_args, &tid, &args, BPF_ANY);
  return 0;
}

static __always_inline int
emitSuccessfulSetuid(struct trace_event_raw_sys_exit *ctx) {
  const __u32 tid = (__u32)bpf_get_current_pid_tgid();
  struct SetuidArgs *args = bpf_map_lookup_elem(&setuid_args, &tid);
  if (!args)
    return 0;
  const struct SetuidArgs savedArgs = *args;
  bpf_map_delete_elem(&setuid_args, &tid);

  if (ctx->ret != 0)
    return 0;
  return emitSetuidEvent(savedArgs.newUid);
}

SEC("tracepoint/syscalls/sys_enter_setuid")
int onSetuid(struct trace_event_raw_sys_enter *ctx) {
  return rememberSetuid((__u32)ctx->args[0]);
}

SEC("tracepoint/syscalls/sys_exit_setuid")
int onSetuidExit(struct trace_event_raw_sys_exit *ctx) {
  return emitSuccessfulSetuid(ctx);
}

SEC("tracepoint/syscalls/sys_enter_setresuid")
int onSetresuid(struct trace_event_raw_sys_enter *ctx) {
  return rememberSetuid((__u32)ctx->args[1]);
}

SEC("tracepoint/syscalls/sys_exit_setresuid")
int onSetresuidExit(struct trace_event_raw_sys_exit *ctx) {
  return emitSuccessfulSetuid(ctx);
}

#endif // VIGIL_PLATFORM_PRIVILEGE_BPF_H
