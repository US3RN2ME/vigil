#include <nlohmann/json.hpp>

#include <vigil/Alert.hpp>

namespace vigil {
namespace detail {
std::string serializeAlert(const Alert &alert, nlohmann::json platformJson);
} // namespace detail

std::string Alert::toJson() const {
  return detail::serializeAlert(
      *this, {
                 {"uid", info.platform.uid},
                 {"euid", info.platform.euid},
                 {"executable_deleted", info.platform.exeDeleted},
                 {"is_memfd", info.platform.isMemfd},
                 {"has_ld_preload", info.platform.hasLdPreload},
                 {"binary_replaced", info.platform.binaryReplaced},
                 {"container_id", info.platform.containerId},
                 {"has_ptrace_attach", info.platform.hasPtraceAttach},
                 {"ptrace_target_pid", info.platform.ptraceTargetPid},
                 {"has_setuid_to_root", info.platform.hasSetuidToRoot},
                 {"has_module_load", info.platform.hasModuleLoad},
             });
}
} // namespace vigil
