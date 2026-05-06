
#ifndef VIGIL_PLATFORM_LINUX_EXECVE_BPF_H
#define VIGIL_PLATFORM_LINUX_EXECVE_BPF_H

struct ExecveArgs {
   char filename[256];
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 8192);
   __type(key, __u32);  // tid
   __type(value, struct ExecveArgs);
} execve_args SEC(".maps");
// clang-format on

SEC("tracepoint/syscalls/sys_enter_execve")
int onExecveEnter(struct trace_event_raw_sys_enter* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct ExecveArgs args = {};
   bpf_probe_read_user_str(args.filename, sizeof(args.filename), (const char*)ctx->args[0]);
   bpf_map_update_elem(&execve_args, &tid, &args, BPF_ANY);
   return 0;
}

SEC("tracepoint/syscalls/sys_exit_execve")
int onExecveExit(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct ExecveArgs* args = bpf_map_lookup_elem(&execve_args, &tid);
   if (!args)
      return 0;
   bpf_map_delete_elem(&execve_args, &tid);

   if (ctx->ret != 0) // exec failed — no new image loaded
      return 0;

   struct ExecveEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_EXECVE);
   // After a successful execve the kernel has already replaced comm with the
   // new program name, so bpf_get_current_comm() in fillHeader gives us the
   // executed binary's short name rather than the calling process name.
   bpf_probe_read_kernel(e->filename, sizeof(e->filename), args->filename);

   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif // VIGIL_PLATFORM_LINUX_EXECVE_BPF_H
