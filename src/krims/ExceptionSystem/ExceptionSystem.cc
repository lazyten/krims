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

#include "ExceptionSystem.hh"
#include "Backtrace.hh"
#include "ExceptionBase.hh"
#include "krims/config.hh"
#include <iostream>

namespace krims {
namespace detail {

[[noreturn]] void terminate_handler() {
  // This makes sure that only the first exception gets handled and
  // further calls to terminate (from other threads) lead to immediate
  // exit in the thread.
  std::call_once(ExceptionSystem::once_handle_exception,
                 ExceptionSystem::do_once_handle_exception);

  std::_Exit(EXIT_FAILURE);  // Exit without cleanup
}
}  // namespace detail

// Initialise static members:
std::once_flag ExceptionSystem::once_init{};
std::once_flag ExceptionSystem::once_handle_exception{};
ExceptionVerbosity ExceptionSystem::verbosity = ExceptionVerbosity::SUMMARY;
std::unique_ptr<char[]> ExceptionSystem::memory{};
size_t ExceptionSystem::mem_offset = 0;

bool ExceptionSystem::initialise(ExceptionVerbosity verbosity) {
  try {
    // clang-tidy flags an error in libstdc++ here, that's why we need the NOLINT
    std::call_once(once_init, &do_once_initialise, verbosity);  // NOLINT
  } catch (const std::system_error& e) {
    std::cerr
          << "Could not setup ExceptionSystem due to std::system_error. \nUsually this "
             "happens if the program is *not* linked to the pthread library."
          << std::endl;
    throw;
  } catch (const std::bad_alloc& e) {
    std::cerr << "Could not allocate memory for ExceptionSystem. Exiting now."
              << std::endl;
    throw;
  }
  return true;
}

void ExceptionSystem::do_once_initialise(ExceptionVerbosity verbosity_) {
  // Allocate pre-allocated heap memory for exception handling
  try {
    memory.reset(new char[max_mem]);
  } catch (...) {
    memory.reset();  // Undo memory allocation if any
    throw std::bad_alloc();
  }

  // Set the other static flags (which will not cause an error)
  std::set_terminate(detail::terminate_handler);
  verbosity = verbosity_;
}

void ExceptionSystem::do_once_handle_exception() noexcept {
  // No need to do anything:
  if (verbosity == ExceptionVerbosity::SILENT) return;

  if (memory == nullptr) {
    // The system has not been initialised properly so far.
    // => We print an error about it and that's all we can do.
    std::cerr
          << "terminate() was called but the krims::ExceptionSystem has not been "
             "initialised properly.\n"
             "This means that no proper exceptiion handling can be done at this point.\n"
             "To avoid this issue call ExceptionSystem::initialise() or load the "
             "ExceptionSystem.hh header \n"
             "with the macro KRIMS_INIT_EXCEPTION_SYSTEM defined.";
    return;
  }

  auto exc = std::current_exception();
  if (exc == nullptr) {
    std::cerr << "terminate() was called." << std::endl;
    return;
  }

  // Separator for the program output and our stuff
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
  if (verbosity == ExceptionVerbosity::BACKTRACE) {
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
}

}  // namespace krims
