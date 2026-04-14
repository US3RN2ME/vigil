
#ifndef VIGIL_LINUX_RINGBUFFER_HPP
#define VIGIL_LINUX_RINGBUFFER_HPP

#include <bpf/libbpf.h>

namespace vigil::linux {
    class RingBuffer {
    public:
        using Callback = ring_buffer_sample_fn; // int(*)(void* ctx, void* data, size_t size)

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

} // namespace vigil::linux

#endif // VIGIL_LINUX_RINGBUFFER_HPP
