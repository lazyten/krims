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

#pragma once
#include "ExceptionVerbosity.hh"
#include "terminate_handler.hh"
#include <exception>
#include <memory>
#include <mutex>

namespace krims {

struct ExceptionSystem {
  template <ExceptionVerbosity verbosity>
  friend void terminate_handler();

 public:
  template <ExceptionVerbosity Verbosity = ExceptionVerbosity::BACKTRACE>
  static bool initialise() {
    std::lock_guard<std::mutex> lock(mutex_);

    /* TODO later
    if (memory_ == nullptr) {
      memory_.reset(new char[max_mem]);
    }
    */

    std::set_terminate(terminate_handler<Verbosity>);
    verbosity_ = Verbosity;

    return true;
  }

  static ExceptionVerbosity verbosity() { return verbosity_; }

  /* TODO include this later: We don't want to allocate new memory
   *      in exception handling (might fail)
  static void* malloc(size_t size) {
    if (size + mem_offset_ < max_mem) {
      std::lock_guard<std::mutex> lock(mutex_);
      void* ret = &memory_[mem_offset_];
      mem_offset_ += size;
      return ret;
    } else {
      return nullptr;
    }
  }
  */

 private:
  /** Currently selected verbosity level */
  static ExceptionVerbosity verbosity_;

  /** Mutex to make operations on the data or exception handling strictly thread-exclusive
   */
  static std::mutex mutex_;

  /* TODO for later
  //! Maximal pre-allocated memory for exception handling.
  static constexpr size_t max_mem = 0;

  //! Buffer holding the pre-allocated memory
  static std::unique_ptr<char[]> memory_;

  //! Offset to the next free memory
  static size_t mem_offset_;
  */
};

#ifdef KRIMS_INIT_EXCEPTION_SYSTEM
// Initialise a (possibly unused) global by calling ExceptionSystem::initialise.
//
// The rationale behind this is, that globals are initialised even *before* the
// main() function gets called and hence that the system is hence up and running
// in case some other global initialisation throws already.
//
// We mark the global such that the compilers do not complain about it being
// unused and we enwrap everything in a macro to make it easier to use it.
// The user just needs to define KRIMS_INIT_EXCEPTION_SYSTEM and include this header
// to get going.

__attribute__((unused)) const bool exception_system_initialised{
      ExceptionSystem::initialise<KRIMS_INIT_EXCEPTION_SYSTEM>()};
#endif  // defined KRIMS_INIT_EXCEPTION_SYSTEM

}  // namespace krims
