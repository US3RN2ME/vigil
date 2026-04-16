
#include "vmlinux.h"
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>

struct ExecveEvent {
    __u32 pid;
    __u32 ppid;
    char comm[16];
    char filename[256];
};

// clang-format off
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1 << 24); // 16MB
} rb SEC(".maps");
// clang-format on

SEC("tracepoint/syscalls/sys_enter_execve")

int onExecve(struct trace_event_raw_sys_enter *ctx) {
    struct ExecveEvent *e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
    if (!e)
        return 0;

    const __u64 pidTgid = bpf_get_current_pid_tgid();
    e->pid = (__u32) (pidTgid >> 32);

    struct task_struct *task = (struct task_struct *) bpf_get_current_task();
    e->ppid = (__u32) BPF_CORE_READ(task, real_parent, tgid);

    bpf_get_current_comm(e->comm, sizeof(e->comm));

    bpf_probe_read_user_str(e->filename, sizeof(e->filename), (const char *) ctx->args[0]);

    bpf_ringbuf_submit(e, 0);
    return 0;
}

char LICENSE[] SEC("license") =
"GPL";
