
#ifndef VIGIL_PLATFORM_MMAP_BPF_H
#define VIGIL_PLATFORM_MMAP_BPF_H

#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define MAP_ANONYMOUS 0x20

struct MmapArgs {
   __u64 prot;
   __u64 flags;
   __s64 fd;
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 1024);
   __type(key, __u32);
   __type(value, struct MmapArgs);
} mmap_args SEC(".maps");
// clang-format on

SEC("tracepoint/syscalls/sys_enter_mmap")
int onMmapEnter(struct trace_event_raw_sys_enter* ctx) {
   const __u64 prot = (__u64)ctx->args[2];
   const __u64 flags = (__u64)ctx->args[3];
   const __s64 fd = (__s64)ctx->args[4];

   if (!(prot & PROT_WRITE) || !(prot & PROT_EXEC))
      return 0;
   if (!(flags & MAP_ANONYMOUS) || fd != -1)
      return 0;

   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct MmapArgs args = {.prot = prot, .flags = flags, .fd = fd};
   bpf_map_update_elem(&mmap_args, &tid, &args, BPF_ANY);
   return 0;
}

SEC("tracepoint/syscalls/sys_exit_mmap")
int onMmapExit(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct MmapArgs* args = bpf_map_lookup_elem(&mmap_args, &tid);
   if (!args)
      return 0;
   bpf_map_delete_elem(&mmap_args, &tid);

   if ((__s64)ctx->ret < 0)
      return 0;

   struct MmapEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_MMAP);
   bpf_ringbuf_submit(e, 0);
   return 0;
}

SEC("tracepoint/syscalls/sys_enter_mprotect")
int onMprotectEnter(struct trace_event_raw_sys_enter* ctx) {
   const __u64 prot = (__u64)ctx->args[2];
   if (!(prot & PROT_WRITE) || !(prot & PROT_EXEC))
      return 0;

   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct MmapArgs args = {.prot = prot};
   bpf_map_update_elem(&mmap_args, &tid, &args, BPF_ANY);
   return 0;
}

SEC("tracepoint/syscalls/sys_exit_mprotect")
int onMprotectExit(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct MmapArgs* args = bpf_map_lookup_elem(&mmap_args, &tid);
   if (!args)
      return 0;
   bpf_map_delete_elem(&mmap_args, &tid);

   if (ctx->ret != 0)
      return 0;

   struct MmapEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_MMAP);
   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif
