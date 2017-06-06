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
#include <iterator>
#include <krims/Algorithm/split.hh>
#include <krims/FileSystem.hh>
#include <numeric>
#include <random>
#include <rapidcheck.h>

namespace krims {
namespace tests {

struct Path {
  // If an absolute path the first entry should be "/".
  //
  // All other entries which are "" or "/" are interpreted as
  // an extra "/" in the path, e.g.
  //     {"/", "a", "/", "", "b" }
  // yields the path
  //     /a///b
  std::vector<std::string> dirs;
  std::string base;
  std::string ext;

  //! Build the directory only
  std::string dir() const {
    std::string ret;
    for (const auto& d : dirs) ret.append(d == "/" || ret == "/" ? d : "/" + d);
    return ret;
  }

  //! Build the file name only
  std::string file() const { return ext.empty() ? base : base + "." + ext; }

  //! Build the full path
  std::string build() const {
    std::string ret = dir();
    if (!ret.empty()) ret.append("/");
    ret.append(file());
    return ret;
  }

  Path(std::vector<std::string> dirs_, std::string base_, std::string ext_)
        : dirs(std::move(dirs_)), base(std::move(base_)), ext(std::move(ext_)) {}
  Path() = default;

  /** Return a path struct representing the path to this very file */
  static Path this_file() {
    Path r;
    const std::string path = realpath(std::string(__FILE__));
    const std::string bn   = basename(path);
    const std::pair<std::string, std::string> base_ext = splitext(bn);

    r.base = base_ext.first;
    if (!base_ext.second.empty()) {
      r.ext = base_ext.second.substr(1);
    }

    std::back_insert_iterator<std::vector<std::string>> backit(r.dirs);
    split(dirname(path), backit, '/');
    return r;
  }
};

std::ostream& operator<<(std::ostream& o, const Path& p) {
  o << p.build();
  return o;
}

rc::Gen<char> gen_ascii_print() {
  static const std::vector<char> ascii_print = [] {
    std::vector<char> ret(127 - 32);
    std::iota(std::begin(ret), std::end(ret), 32);
    return ret;
  }();
  return rc::gen::elementOf(ascii_print);
}

/* pathpart: Chars for dirs and base (filename without extension)
 *   - more dots than normal, but no slashes -> tests whether . in pathnames do not mess
*   up extension. */
rc::Gen<std::string> gen_pathpart() {
  using namespace rc;
  auto not_slash        = gen::distinctFrom(gen_ascii_print(), '/');
  auto pathpart_chargen = gen::weightedOneOf<char>({{8, not_slash}, {1, gen::just('.')}});
  return gen::nonEmpty(gen::container<std::string>(std::move(pathpart_chargen)));
}

rc::Gen<Path> gen_path(size_t n_dirs, size_t ext_len) {
  using namespace rc;

  auto not_slash     = gen::distinctFrom(gen_ascii_print(), '/');
  auto not_slash_dot = gen::distinctFrom(not_slash, '.');

  // basepart: (Filename without extension)
  //   - If the extension length is 0: No dots in the string
  //   - The first char is a . with higher probability (unix hidden file,
  //     i.e. no extension)
  auto basepart_1char_gen =
        gen::weightedOneOf<char>({{1, std::move(not_slash)}, {1, gen::just('.')}});
  auto no_slash_dot_string_gen =
        gen::nonEmpty(gen::container<std::string>(not_slash_dot));
  auto pair_gen = gen::pair(std::move(basepart_1char_gen),
                            ext_len > 0 ? gen_pathpart() : no_slash_dot_string_gen);
  auto basepart_gen =
        gen::map(std::move(pair_gen), [](std::pair<char, std::string> c_str) {
          return std::string(1, c_str.first) + c_str.second;
        });

  // Extension: If larger than zero, than generate a string without slashes and dots.
  auto ext_gen = gen::container<std::string>(ext_len, std::move(not_slash_dot));

  return gen::construct<Path>(
        gen::container<std::vector<std::string>>(n_dirs, gen_pathpart()),
        std::move(basepart_gen), std::move(ext_gen));
}

rc::Gen<Path> gen_path() {
  using namespace rc;

  auto n_dirs_gen = gen::weightedOneOf<size_t>(
        {{10, gen::inRange<size_t>(0, 11)}, {1, gen::just<size_t>(0)}});
  auto ext_len_gen = gen::weightedOneOf<size_t>(
        {{10, gen::inRange<size_t>(0, 6)}, {1, gen::just<size_t>(0)}});
  auto pair_gen = gen::pair(std::move(n_dirs_gen), std::move(ext_len_gen));

  return gen::mapcat(std::move(pair_gen), [](std::pair<size_t, size_t> i) {
    return gen_path(i.first, i.second);
  });
}

TEST_CASE("filesystem tests", "[FileSystem]") {
  SECTION("path_exists") {
    CHECK_FALSE(path_exists("/nonexistent"));
    CHECK_FALSE(path_exists("nonExIstent"));
    CHECK(path_exists("/bin/sh"));
    CHECK(path_exists(std::string(__FILE__)));
    CHECK(path_exists("."));
  }

  SECTION("basename and dirname") {
    auto testable = []() {
      Path p = *gen_path().as("Path to use for testing");

      const std::string dir  = dirname(p.build());
      const std::string base = basename(p.build());
      if (p.dir().empty()) {
        RC_ASSERT(dir == ".");
      } else {
        RC_ASSERT(dir == p.dir());
      }

      RC_ASSERT(base == p.file());
    };
    REQUIRE(rc::check("basename and dirname", testable));
  }  // basename and dirname

  SECTION("splitext") {
    auto testable = []() {
      Path p = *gen_path().as("Path");
      const std::pair<std::string, std::string> res = splitext(p.build());

      const std::string ext  = p.ext.empty() ? "" : "." + p.ext;
      const std::string rest = p.dir().empty() ? p.base : p.dir() + "/" + p.base;
      RC_ASSERT(res.first == rest);
      RC_ASSERT(res.second == ext);
      RC_ASSERT(res.first + res.second == p.build());
    };
    REQUIRE(rc::check("splitext", testable));
  }  // splitext

  SECTION("realpath") {
    CHECK_THROWS_AS(realpath(std::string("/nonexistent")), ExcRealpathError);

    auto testable = [] {
      const Path p = Path::this_file();
      Path cp(p);
      std::default_random_engine dre;

      const int n_ups   = *rc::gen::inRange(0, 6).as("Number of .. constructs to insert");
      const int n_dots  = *rc::gen::inRange(0, 6).as("Number of . constructs to insert");
      const int n_dupls = *rc::gen::inRange(0, 6).as("Number of duplicate / to insert");

      // TODO Test symbolic link resolution!
      //      For this we need the ability to create symbolic links,
      //      which we lack at the moment.

      for (int i = 0; i < n_ups; ++i) {
        // TODO Ideally one would want this with arbitrary paths,
        // but for this we need the ability to create directories,
        // which we lack atm.
        // const std::string dummy_path =
        //      *gen_pathpart().as("Path " + std::to_string(i) + " to insert");
        const std::string dummy_path = cp.dirs.back();
        cp.dirs.emplace_back("..");
        cp.dirs.push_back(dummy_path);
      }

      for (int i = 0; i < n_dots; ++i) {
        std::uniform_int_distribution<ptrdiff_t> randpos(
              i, static_cast<ptrdiff_t>(cp.dirs.size()));
        auto it = std::begin(cp.dirs) + randpos(dre);
        cp.dirs.insert(it, ".");
      }

      for (int i = 0; i < n_dupls; ++i) {
        std::uniform_int_distribution<ptrdiff_t> randpos(
              i, static_cast<ptrdiff_t>(cp.dirs.size()));
        auto it = std::begin(cp.dirs) + randpos(dre);
        cp.dirs.insert(it, "/");
      }
      RC_LOG() << "Final path passed to realpath: " << cp << std::endl;

      const std::string res = realpath(cp.build());
      RC_ASSERT(res == p.build());
    };
    REQUIRE(rc::check("realpath", testable));
  }  // realpath

  // TODO:
  //   - Test for memory leaks!

}  // FileSystem

}  // namespace tests
}  // namespace krims
