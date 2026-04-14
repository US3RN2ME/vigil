
#include "BpfObject.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <sys/resource.h>

namespace vigil::linux {
    BpfObject::BpfObject(std::string_view path) {
        obj_ = bpf_object__open(path.data());
        if (!obj_)
            throw std::runtime_error{"bpf_object__open failed"};
    }

    BpfObject::~BpfObject() {
        if (obj_)
            bpf_object__close(obj_);
    }

    BpfObject::BpfObject(BpfObject&& o) noexcept : obj_{std::exchange(o.obj_, nullptr)} {}

    BpfObject& BpfObject::operator=(BpfObject&& o) noexcept {
        if (this != &o) {
            if (obj_)
                bpf_object__close(obj_);
            obj_ = std::exchange(o.obj_, nullptr);
        }
        return *this;
    }

    void BpfObject::load() {
        const rlimit rl{
            .rlim_cur = RLIM_INFINITY,
            .rlim_max = RLIM_INFINITY
        };
        if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0)
            std::cerr << "failed to raise RLIMIT_MEMLOCK" << std::endl;
        if (bpf_object__load(obj_))
            throw std::runtime_error{"bpf_object__load failed"};
    }

    void BpfObject::attach(std::string_view programName) {
        struct bpf_program* prog = bpf_object__find_program_by_name(obj_, programName.data());
        if (!prog)
            throw std::runtime_error{"bpf program not found: " + std::string{programName}};
        bpf_program__attach(prog);
    }

    int BpfObject::mapFd(std::string_view mapName) const {
        struct bpf_map* map = bpf_object__find_map_by_name(obj_, mapName.data());
        if (!map)
            throw std::runtime_error{"bpf map not found: " + std::string{mapName}};
        return bpf_map__fd(map);
    }

} // namespace vigil::linux
