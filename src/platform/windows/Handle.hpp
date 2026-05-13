
#ifndef VIGIL_PLATFORM_HANDLE_HPP
#define VIGIL_PLATFORM_HANDLE_HPP

namespace vigil::platform {
   class Handle {
   public:
      using NativeType = void*;

      Handle() noexcept = default;
      explicit Handle(NativeType handle) noexcept;
      ~Handle();

      Handle(const Handle&) = delete;
      Handle& operator=(const Handle&) = delete;

      Handle(Handle&& other) noexcept;
      Handle& operator=(Handle&& other) noexcept;

      [[nodiscard]] NativeType native() const noexcept;
      [[nodiscard]] bool isValid() const noexcept;
      [[nodiscard]] explicit operator bool() const noexcept;

      void reset(NativeType handle = nullptr) noexcept;
      [[nodiscard]] NativeType release() noexcept;

   private:
      NativeType handle_{nullptr};
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_HANDLE_HPP
