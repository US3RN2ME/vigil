
#include "RingBuffer.hpp"

#include <functional>
#include <stdexcept>
#include <utility>

namespace vigil::linux {
    RingBuffer::RingBuffer(int mapFd, Callback cb, void* ctx) {
        rb_ = ring_buffer__new(mapFd, cb, ctx, nullptr);
        if (!rb_)
            throw std::runtime_error{"ring_buffer__new failed"};
    }

    RingBuffer::~RingBuffer() {
        if (rb_)
            ring_buffer__free(rb_);
    }

    RingBuffer::RingBuffer(RingBuffer&& o) noexcept : rb_{std::exchange(o.rb_, nullptr)} {}

    RingBuffer& RingBuffer::operator=(RingBuffer&& o) noexcept {
        if (this != &o) {
            if (rb_)
                ring_buffer__free(rb_);
            rb_ = std::exchange(o.rb_, nullptr);
        }
        return *this;
    }

    int RingBuffer::poll(int timeoutMs) const {
        return ring_buffer__poll(rb_, timeoutMs);
    }
} // namespace vigil::linux