#ifndef VIGIL_PLATFORM_CONNECT_BPF_H
#define VIGIL_PLATFORM_CONNECT_BPF_H

#define AF_INET 2
#define AF_INET6 10
#define EINPROGRESS 115

struct ConnectArgs {
   __u16 sa_family;
   __u16 dport;
   __u8 daddr[16];
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_HASH);
   __uint(max_entries, 1024);
   __type(key, __u32);  // tid
   __type(value, struct ConnectArgs);
} connect_args SEC(".maps");
// clang-format on

SEC("tracepoint/syscalls/sys_enter_connect")
int onConnectEnter(struct trace_event_raw_sys_enter* ctx) {
   const struct sockaddr* uaddr = (const struct sockaddr*)ctx->args[1];
   if (!uaddr)
      return 0;

   __u16 family = 0;
   if (bpf_probe_read_user(&family, sizeof(family), uaddr))
      return 0;

   if (family != AF_INET && family != AF_INET6)
      return 0;

   struct ConnectArgs args = {};
   args.sa_family = family;

   if (family == AF_INET) {
      struct sockaddr_in sin = {};
      if (bpf_probe_read_user(&sin, sizeof(sin), uaddr))
         return 0;

      const __u8 first = ((__u8*)(&sin.sin_addr.s_addr))[0];
      if (first == 127)
         return 0;

      args.dport = __builtin_bswap16(sin.sin_port);
      __builtin_memcpy(args.daddr, &sin.sin_addr.s_addr, 4);
   } else {
      struct sockaddr_in6 sin6 = {};
      if (bpf_probe_read_user(&sin6, sizeof(sin6), uaddr))
         return 0;

      args.dport = __builtin_bswap16(sin6.sin6_port);
      __builtin_memcpy(args.daddr, sin6.sin6_addr.in6_u.u6_addr8, 16);
   }

   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   bpf_map_update_elem(&connect_args, &tid, &args, BPF_ANY);
   return 0;
}

SEC("tracepoint/syscalls/sys_exit_connect")
int onConnectExit(struct trace_event_raw_sys_exit* ctx) {
   const __u32 tid = (__u32)bpf_get_current_pid_tgid();
   struct ConnectArgs* args = bpf_map_lookup_elem(&connect_args, &tid);
   if (!args)
      return 0;
   bpf_map_delete_elem(&connect_args, &tid);

   const long ret = ctx->ret;
   if (ret != 0 && ret != -EINPROGRESS)
      return 0;

   struct ConnectEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_CONNECT);
   e->sa_family = args->sa_family;
   e->dport = args->dport;
   __builtin_memcpy(e->daddr, args->daddr, 16);

   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif // VIGIL_PLATFORM_CONNECT_BPF_H
