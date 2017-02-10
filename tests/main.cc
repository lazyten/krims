//
// Copyright (C) 2016 by the krims authors
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

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <krims/ExceptionSystem.hh>

// Use the usual trick to get the compiler to initialise the exception system
// as early as possible.
const bool init_exception_system{krims::ExceptionSystem::initialise<>()};

int main(int argc, char* const argv[]) {
  // Run catch:
  int result = Catch::Session().run(argc, argv);
  return result;
}
