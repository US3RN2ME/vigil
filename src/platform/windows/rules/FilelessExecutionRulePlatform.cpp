#include <vigil/rules/FilelessExecutionRule.hpp>

namespace vigil::rules::detail {
   bool isMemfdProcess(const ProcessInfo& info) {
      (void)info;
      return false;
   }

   bool isDeletedExecutable(const ProcessInfo& info) {
      (void)info;
      return false;
   }
} // namespace vigil::rules::detail
