#include <vigil/rules/FilelessExecutionRule.hpp>

namespace vigil::rules::detail {
bool isMemfdProcess(const ProcessInfo &info) { return info.platform.isMemfd; }

bool isDeletedExecutable(const ProcessInfo &info) {
  return info.platform.exeDeleted;
}
} // namespace vigil::rules::detail
