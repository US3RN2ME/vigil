
#ifndef VIGIL_PLATFORM_WINDOWS_STRINGUTILS_HPP
#define VIGIL_PLATFORM_WINDOWS_STRINGUTILS_HPP

#include <string>
#include <string_view>

namespace vigil::platform::windows {
   class StringUtils {
   public:
      static std::string wideToUtf8(const wchar_t* wide, int size);
      static std::string filenameFromPath(std::string_view path);
   };
} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_WINDOWS_STRINGUTILS_HPP
