#include "BpfObject.hpp"

#include <bpf/libbpf.h>
#include <sys/resource.h>
#include <utility>

#include <vigil/Error.hpp>
#include <vigil/Logger.hpp>

namespace vigil::platform::linux {
   BpfObject::BpfObject(std::string_view path) {
      obj_ = bpf_object__open(path.data());
      if (!obj_)
         throw CollectorError{"bpf_object__open failed for '{}'", path};

      log::info("opened BPF object {}", path);
   }

   BpfObject::~BpfObject() {
      if (obj_)
         bpf_object__close(obj_);
   }

   BpfObject::BpfObject(BpfObject&& o) noexcept
       : obj_{std::exchange(o.obj_, nullptr)} {}

   BpfObject& BpfObject::operator=(BpfObject&& o) noexcept {
      if (this != &o) {
         if (obj_)
            bpf_object__close(obj_);
         obj_ = std::exchange(o.obj_, nullptr);
      }
      return *this;
   }

   void BpfObject::load() {
      const rlimit rl{.rlim_cur = RLIM_INFINITY, .rlim_max = RLIM_INFINITY};
      if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0)
         log::warn("failed to raise RLIMIT_MEMLOCK");

      if (bpf_object__load(obj_))
         throw CollectorError{"bpf_object__load failed"};

      log::info("BPF object loaded successfully");
   }

   void BpfObject::attach(std::string_view programName) {
      struct bpf_program* prog = bpf_object__find_program_by_name(obj_, programName.data());
      if (!prog)
         throw CollectorError{"bpf program not found: '{}'", programName};

      if (bpf_program__attach(prog) == nullptr)
         throw CollectorError{"bpf_program__attach failed for '{}'", programName};

      log::info("attached BPF program {}", programName);
   }

   int BpfObject::mapFd(std::string_view mapName) const {
      struct bpf_map* map = bpf_object__find_map_by_name(obj_, mapName.data());
      if (!map)
         throw CollectorError{"bpf map not found: '{}'", mapName};

      return bpf_map__fd(map);
   }
} // namespace vigil::platform::linux
