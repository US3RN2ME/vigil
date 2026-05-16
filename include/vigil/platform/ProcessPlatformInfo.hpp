#ifndef VIGIL_PLATFORM_PROCESSPLATFORMINFO_HPP
#define VIGIL_PLATFORM_PROCESSPLATFORMINFO_HPP

#if defined(VIGIL_PLATFORM_WINDOWS)
#include <vigil/platform/windows/ProcessPlatformInfo.hpp>
#elif defined(VIGIL_PLATFORM_LINUX)
#include <vigil/platform/linux/ProcessPlatformInfo.hpp>
#else
#error "Unsupported platform for vigil::platform::ProcessPlatformInfo"
#endif

#endif // VIGIL_PLATFORM_PROCESSPLATFORMINFO_HPP
