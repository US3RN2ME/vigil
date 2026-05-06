#ifndef VIGIL_PLATFORM_LINUX_COMMON_BPF_H
#define VIGIL_PLATFORM_LINUX_COMMON_BPF_H

// clang-format off
#include "vmlinux.h"

#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
// clang-format on

enum EventType {
   EVENT_EXECVE = 1,
   EVENT_MMAP = 2,
   EVENT_CONNECT = 3,
   EVENT_PTRACE = 4,
   EVENT_SETUID = 5,
   EVENT_MODULE = 6,
};

struct EventHeader {
   __u32 type; // enum EventType
   __u32 pid;
   __u32 ppid;
   char comm[16];
};

struct ExecveEvent {
   struct EventHeader hdr;
   char filename[256];
};

struct MmapEvent {
   struct EventHeader hdr;
};

struct ConnectEvent {
   struct EventHeader hdr;
   __u16 sa_family;
   __u16 dport;
   __u8 daddr[16];
};

struct PtraceEvent {
   struct EventHeader hdr;
   __u32 targetPid;
   __u64 request;
};

struct SetuidEvent {
   struct EventHeader hdr;
   __u32 newUid;
};

struct ModuleEvent {
   struct EventHeader hdr;
};

// clang-format off
struct {
   __uint(type, BPF_MAP_TYPE_RINGBUF);
   __uint(max_entries, 1 << 24); // 16 MB
} rb SEC(".maps");
// clang-format on

static __always_inline void fillHeader(struct EventHeader* hdr, __u32 type) {
   hdr->type = type;
   const __u64 pidTgid = bpf_get_current_pid_tgid();
   hdr->pid = (__u32)(pidTgid >> 32);
   hdr->ppid = (__u32)BPF_CORE_READ((struct task_struct*)bpf_get_current_task(), real_parent, tgid);
   bpf_get_current_comm(hdr->comm, sizeof(hdr->comm));
}

#endif // VIGIL_PLATFORM_LINUX_COMMON_BPF_H
