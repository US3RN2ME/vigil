
#ifndef VIGIL_PLATFORM_LINUX_BPFSKELETON_HPP
#define VIGIL_PLATFORM_LINUX_BPFSKELETON_HPP

struct events_bpf;

namespace vigil::platform::linux {
   class BpfSkeleton {
   public:
      BpfSkeleton();
      ~BpfSkeleton();

      BpfSkeleton(const BpfSkeleton&) = delete;
      BpfSkeleton& operator=(const BpfSkeleton&) = delete;
      BpfSkeleton(BpfSkeleton&&) noexcept;
      BpfSkeleton& operator=(BpfSkeleton&&) noexcept;

      void load();
      void attach();
      [[nodiscard]] int ringBufFd() const;

   private:
      struct events_bpf* skel_{};
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_BPFSKELETON_HPP