#ifndef VIGIL_PLATFORM_LINUX_CONNECT_BPF_H
#define VIGIL_PLATFORM_LINUX_CONNECT_BPF_H

#define AF_INET 2
#define AF_INET6 10

SEC("tracepoint/syscalls/sys_enter_connect")
int onConnect(struct trace_event_raw_sys_enter* ctx) {
   const struct sockaddr* uaddr = (const struct sockaddr*)ctx->args[1];
   if (!uaddr)
      return 0;

   __u16 family = 0;
   if (bpf_probe_read_user(&family, sizeof(family), uaddr))
      return 0;

   if (family != AF_INET && family != AF_INET6)
      return 0;

   __u16 dport = 0;
   __u8 daddr[16];
   __builtin_memset(daddr, 0, sizeof(daddr));

   if (family == AF_INET) {
      struct sockaddr_in sin = {};
      if (bpf_probe_read_user(&sin, sizeof(sin), uaddr))
         return 0;

      const __u8 first = ((__u8*)(&sin.sin_addr.s_addr))[0];
      if (first == 127)
         return 0;

      dport = __builtin_bswap16(sin.sin_port);
      __builtin_memcpy(daddr, &sin.sin_addr.s_addr, 4);
   } else {
      struct sockaddr_in6 sin6 = {};
      if (bpf_probe_read_user(&sin6, sizeof(sin6), uaddr))
         return 0;

      dport = __builtin_bswap16(sin6.sin6_port);
      __builtin_memcpy(daddr, sin6.sin6_addr.in6_u.u6_addr8, 16);
   }

   struct ConnectEvent* e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
   if (!e)
      return 0;

   fillHeader(&e->hdr, EVENT_CONNECT);
   e->sa_family = family;
   e->dport = dport;
   __builtin_memcpy(e->daddr, daddr, 16);

   bpf_ringbuf_submit(e, 0);
   return 0;
}

#endif // VIGIL_PLATFORM_LINUX_CONNECT_BPF_H
