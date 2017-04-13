//
// Copyright (C) 2017 by the krims authors
//
// This file is part of krims.
//
// krims is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// krims is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with krims. If not, see <http://www.gnu.org/licenses/>.
//

#include <catch.hpp>
#include <krims/ExceptionSystem/Backtrace.hh>
#include <krims/macros/noinline.hh>

namespace krims {
namespace tests {

namespace detail {
KRIMS_NOINLINE int inner(Backtrace& b, bool expensive) {
  b.obtain_backtrace(expensive);
  return 1;
}

KRIMS_NOINLINE int middle(Backtrace& b, bool expensive) { return inner(b, expensive); }
}  // namespace detail

KRIMS_NOINLINE int outer(Backtrace& b, bool expensive) {
  return detail::middle(b, expensive);
}

#define CHECK_END(s, t)                                        \
  {                                                            \
    CHECK(s.substr(s.size() - std::min(s.size(), t.size())) == \
          t.substr(t.size() - std::min(s.size(), t.size())));  \
  }

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
TEST_CASE("Backtrace", "[backtrace]") {
/** The name of the executable */
#ifdef DEBUG
  const std::string executable_end = "krims_tests_DEBUG";
#else
  const std::string executable_end = "krims_tests_RELEASE";
#endif

  /** The addresses which refer to the individual functions */
  const auto addresses = []() {
    Backtrace b;
    outer(b, false);
    const auto& frms = b.frames();
    return std::array<std::string, 3>{
          {frms[0].address, frms[1].address, frms[2].address}};
  }();

  /** The number of addresses == number of frames */
  constexpr size_t n = addresses.size();

/** The function names */
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
  const std::array<std::string, n> funnames{{
        "krims::tests::detail::inner(krims::Backtrace&, bool)",
        "krims::tests::detail::middle(krims::Backtrace&, bool)",
        "krims::tests::outer(krims::Backtrace&, bool)",
  }};
#else
  // Here we will only get the mangled names ...
  // TODO do something more sensible here
  const std::array<std::string, n> funnames{
        "not yet implemented", "not yet implemented", "not yet implemented",
  };
#endif  // KRIMS_HAVE_LIBSTDCXX_DEMANGLER

  // The frame information we expect to get.
  const std::array<Backtrace::Frame, n> expected{{
        {executable_end, addresses[0], funnames[0], "tests/BacktraceTests.cc", "29"},
        {executable_end, addresses[1], funnames[1], "tests/BacktraceTests.cc", "33"},
        {executable_end, addresses[2], funnames[2], "tests/BacktraceTests.cc", "37"},
  }};

  SECTION("Expensive test") {
    // Call some function stack and obtain an
    // expensive backtrace inside it:
    Backtrace b;
    outer(b, true);
    INFO("Full backtrace:" << '\n' << b << '\n');

#ifdef KRIMS_ADDR2LINE_AVAILABLE
    CHECK(b.determine_file_line());
#else
    CHECK_FALSE(b.determine_file_line());
#endif  // KRIMS_ADDR2LINE_AVAILABLE

    size_t i = 0;
    for (auto it = std::begin(expected); it != std::end(expected); ++it, ++i) {
      INFO("Dealing with frame " << i << ".");

      CHECK_END(b.frames()[i].executable_name, it->executable_name);
      CHECK(b.frames()[i].function_name == it->function_name);
      CHECK(b.frames()[i].address == it->address);

#ifdef KRIMS_ADDR2LINE_AVAILABLE
      CHECK(b.frames()[i].line_number == it->line_number);
      CHECK_END(b.frames()[i].codefile, it->codefile);
#else
      CHECK(b.frames()[i].line_number == Backtrace::Frame::unknown);
      CHECK(b.frames()[i].codefile == Backtrace::Frame::unknown);
#endif  // KRIMS_ADDR2LINE_AVAILABLE
    }   // for
  }     // expensive

  SECTION("Cheap test") {
    // Call some function stack and obtain a
    // cheap backtrace inside it:
    Backtrace b;
    outer(b, false);
    INFO("Full backtrace:" << '\n' << b << '\n');

    CHECK_FALSE(b.determine_file_line());
    size_t i = 0;
    for (auto it = std::begin(expected); it != std::end(expected); ++it, ++i) {
      INFO("Dealing with frame " << i << ".");

      CHECK_END(b.frames()[i].executable_name, it->executable_name);
      CHECK(b.frames()[i].function_name == it->function_name);
      CHECK(b.frames()[i].address == it->address);

      CHECK(b.frames()[i].line_number == Backtrace::Frame::unknown);
      CHECK(b.frames()[i].codefile == Backtrace::Frame::unknown);
    }  // for
  }    // cheap
}  // Backtrace
#else
TEST_CASE("Backtrace", "[backtrace]") {
  // If KRIMS_HAVE_GLIBC_STACKTRACE is not set, than there is no way to obtain
  // backtrace frames, hence we just check that the number of obtained frames
  // is really exactly zero
  Backtrace b;
  outer(b, false);
  CHECK(b.frames().size() == 0);
}  // Backtrace

#endif  // KRIMS_HAVE_GLIBC_STACKTRACE

}  // namespace tests
}  // namespace krims
