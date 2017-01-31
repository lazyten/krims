//
// Copyright (C) 2016-17 by the krims authors
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

#include <krims/ExceptionSystem.hh>
#include <krims/SubscriptionPointer.hh>
#include <krims/version.hh>
#include <sstream>
#include <vector>

using namespace krims;

void print_error(int partmax) {
  std::cerr << "Need a number between " << 0 << " and " << partmax
            << " as the only argument: The exception which is triggered." << std::endl;
}

void part0() {
  int i = -3;
  // Assert that i is greater than 0
  assert_greater(0, i);
}

void part1() {
  int i = 5;
  // Assert that i is between 0 and 5
  // assert_range asserts the upper bound plus 1
  assert_range(0, i, 6);

  i = 6;
  assert_range(0, i, 6);
}

void part2() {
  int i = 5;
  int j = 5;

  // Assert that i and j are equal.
  assert_equal(i, j);

  i = 6;
  assert_equal(i, j);
}

void part3() {
  // Just abort after an internal error.
  assert_dbg(false, ExcInternalError());
}

void part4() {
  // Abort if a non-finite number is encountered.
  double i = 3. / 0.;
  assert_finite(i);
}

void part5() {
  std::vector<std::vector<int>> vvec{{0, 1}, {2, 3, 4}};

  // Check that all containers in vvec have size 2:
  assert_element_sizes(vvec, 2);
}

class TestclassPart6 : public krims::Subscribable {};
void part6() {
  krims::SubscriptionPointer<TestclassPart6> bptr("part6");
  TestclassPart6 b;
  bptr.reset(b);
}

int main(int argc, char** argv) {
  std::cout << "Using krims version " << krims::version::version_string() << std::endl;

  const int partmax = 6;

  if (argc != 2) {
    print_error(partmax);
    return 1;
  }

  int part;
  std::stringstream ss(argv[1]);
  if (!(ss >> part) || part < 0 || part > partmax) {
    print_error(partmax);
    return 1;
  }

  if (part-- == 0) part0();
  if (part-- == 0) part1();
  if (part-- == 0) part2();
  if (part-- == 0) part3();
  if (part-- == 0) part4();
  if (part-- == 0) part5();
  if (part-- == 0) part6();

  return 0;
}
