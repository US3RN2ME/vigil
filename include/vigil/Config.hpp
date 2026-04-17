
#ifndef VIGIL_CONFIG_HPP
#define VIGIL_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vigil/rules/RuleConfig.hpp>

namespace vigil {
   class Config {
  public:
      [[nodiscard]] static Config loadFromFile(std::string_view path);

      [[nodiscard]] rules::RuleConfig rule(std::string_view path) const noexcept;

  private:
      std::unordered_map<std::string, rules::RuleConfig> rules_;
   };
} // namespace vigil

#endif // VIGIL_CONFIG_HPP
