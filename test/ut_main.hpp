
#ifndef VIGIL_TEST_UT_MAIN_HPP
#define VIGIL_TEST_UT_MAIN_HPP

#include <boost/ut.hpp>

using namespace boost::ut;

int main() {
   return cfg<override>.run();
}

#endif //VIGIL_TEST_UT_MAIN_HPP
