#include "RingBuffer.hpp"

#include <bpf/libbpf.h>
#include <utility>

#include <vigil/Error.hpp>
#include <vigil/Logger.hpp>

namespace vigil::platform {
RingBuffer::RingBuffer(int mapFd, Callback cb, void *ctx) {
  rb_ = ring_buffer__new(mapFd, cb, ctx, nullptr);
  if (!rb_)
    throw CollectorError{"ring_buffer__new failed"};
  log::info("ring buffer created");
}

RingBuffer::~RingBuffer() {
  if (rb_)
    ring_buffer__free(rb_);
}

RingBuffer::RingBuffer(RingBuffer &&o) noexcept
    : rb_{std::exchange(o.rb_, nullptr)} {}

RingBuffer &RingBuffer::operator=(RingBuffer &&o) noexcept {
  if (this != &o) {
    if (rb_)
      ring_buffer__free(rb_);
    rb_ = std::exchange(o.rb_, nullptr);
  }
  return *this;
}

int RingBuffer::poll(int timeoutMs) const {
  const auto rc = ring_buffer__poll(rb_, timeoutMs);
  if (rc < 0)
    log::warn("ring_buffer__poll returned {}", rc);
  return rc;
}
} // namespace vigil::platform
