#include <catch.hpp>
#include <iostream>
#include <krims/version.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {

TEST_CASE("Dummy test", "[dummy]") {

  // just to see if linking works
  std::cout << "krims version: " << version::version_string() << std::endl;

  auto test = [](int x) { RC_ASSERT(x + x == 2 * x); };
  CHECK(rc::check("Run dummy", test));
}

}  // namespace tests
}  // namespace krims
