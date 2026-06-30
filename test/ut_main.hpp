
#ifndef VIGIL_TEST_UT_MAIN_HPP
#define VIGIL_TEST_UT_MAIN_HPP

#include <boost/ut.hpp>
#include <string>
#include <string_view>

using namespace boost::ut;

template <class Attributes>
const std::string* attrValue(const Attributes& attributes, std::string_view key) {
   for (const auto& [attrKey, value] : attributes) {
      if (attrKey == key)
         return &value;
   }
   return nullptr;
}

int main() {
   return cfg<override>.run();
}

#endif // VIGIL_TEST_UT_MAIN_HPP
