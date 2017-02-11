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

#include "terminate_handler.hh"
#include "Backtrace.hh"
#include "ExceptionBase.hh"
#include "ExceptionSystem.hh"
#include <iostream>

namespace krims {

template <ExceptionVerbosity Verbosity>
[[noreturn]] void terminate_handler() {
  if (Verbosity == ExceptionVerbosity::SILENT) {
    // Exit without cleanup.
    std::_Exit(EXIT_FAILURE);
  }

  // Separate the program output and our stuff

  auto exc = std::current_exception();
  if (exc == nullptr) {
    std::cerr << "terminate() was called." << std::endl;
    std::_Exit(EXIT_FAILURE);
  }

  // Lock exception system for us:
  std::lock_guard<std::mutex> lock(ExceptionSystem::mutex_);
  const char* sep = "---------------------------------------------------------";

  std::cerr << '\n'
            << sep << '\n'
            << "terminate() was called due to an uncaught exception:\n\n";

  try {
    // Rethrow to recognise type and deal with exception:
    std::rethrow_exception(exc);
  } catch (const std::exception& exc) {
    std::cerr << exc.what();
  } catch (...) {
    std::cerr << "Unknown exception.";
  }

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
  // We want and have a backtrace:
  if (Verbosity == ExceptionVerbosity::BACKTRACE) {
    try {
      // TODO Rewrite into something more sensible
      std::cerr << '\n' << sep << '\n';
      std::cerr << "Backtrace:\n";
      Backtrace bt;
      bt.obtain_backtrace(true);
      std::cerr << '\n' << bt;
    } catch (...) {
      // If we catch something from the backtrace step:
      // Ignore it -> we just have no backtrace.
    }   // try
  }     // if
#endif  // KRIMS_HAVE_GLIBC_STACKTRACE

  std::cerr << '\n' << sep << '\n';

  // Return without any further cleanup.
  std::_Exit(EXIT_FAILURE);
}

// Explicitly instantiate a few
template void terminate_handler<ExceptionVerbosity::SILENT>();
template void terminate_handler<ExceptionVerbosity::SUMMARY>();
template void terminate_handler<ExceptionVerbosity::BACKTRACE>();

}  // namespace krims
