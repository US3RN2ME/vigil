#include "BpfSkeleton.hpp"

#include <sys/resource.h>
#include <utility>

#include "events.skel.h"

#include <vigil/Error.hpp>
#include <vigil/Logger.hpp>

namespace vigil::platform {
   BpfSkeleton::BpfSkeleton() {
      skel_ = events_bpf__open();
      if (!skel_)
         throw CollectorError{"events_bpf__open failed"};
   }

   BpfSkeleton::~BpfSkeleton() {
      if (skel_)
         events_bpf__destroy(skel_);
   }

   BpfSkeleton::BpfSkeleton(BpfSkeleton&& o) noexcept
       : skel_{std::exchange(o.skel_, nullptr)} {}

   BpfSkeleton& BpfSkeleton::operator=(BpfSkeleton&& o) noexcept {
      if (this != &o) {
         if (skel_)
            events_bpf__destroy(skel_);
         skel_ = std::exchange(o.skel_, nullptr);
      }
      return *this;
   }

   void BpfSkeleton::load() {
      const rlimit rl{.rlim_cur = RLIM_INFINITY, .rlim_max = RLIM_INFINITY};
      if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0)
         log::warn("failed to raise RLIMIT_MEMLOCK");

      if (events_bpf__load(skel_))
         throw CollectorError{"events_bpf__load failed"};

      log::info("BPF skeleton loaded");
   }

   void BpfSkeleton::attach() {
      if (events_bpf__attach(skel_))
         throw CollectorError{"events_bpf__attach failed"};

      log::info("BPF programs attached");
   }

   int BpfSkeleton::ringBufFd() const {
      return bpf_map__fd(skel_->maps.rb);
   }
} // namespace vigil::platform
