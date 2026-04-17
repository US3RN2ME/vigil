
#ifndef VIGIL_PLATFORM_LINUX_RINGBUFFER_HPP
#define VIGIL_PLATFORM_LINUX_RINGBUFFER_HPP

#include <cstddef>

struct ring_buffer;

namespace vigil::platform::linux {
   class RingBuffer {
   public:
      using Callback = int (*)(void* ctx, void* data, size_t size);

      RingBuffer(int mapFd, Callback cb, void* ctx);

      ~RingBuffer();

      RingBuffer(const RingBuffer&) = delete;

      RingBuffer& operator=(const RingBuffer&) = delete;

      RingBuffer(RingBuffer&& o) noexcept;

      RingBuffer& operator=(RingBuffer&& o) noexcept;

      int poll(int timeoutMs = 100) const;

   private:
      struct ring_buffer* rb_ = nullptr;
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_RINGBUFFER_HPP
