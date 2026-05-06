// clang-format off
#include "vmlinux.h"

#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>

#include "common.bpf.h"
#include "execve.bpf.h"
#include "mmap.bpf.h"
#include "connect.bpf.h"
#include "ptrace.bpf.h"
#include "privilege.bpf.h"
#include "module.bpf.h"
// clang-format on

char LICENSE[] SEC("license") = "GPL";
