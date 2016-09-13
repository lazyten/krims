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

#include "Backtrace.hh"
#include <sstream>

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
#include <execinfo.h>
#endif

#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
#include <cxxabi.h>
#endif

namespace krims {

const std::string Backtrace::Frame::unknown = "?";

Backtrace::Backtrace() : m_parsed_frames{} {
#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
  m_parsed_frames.reserve(n_max_frames);
#endif
}

void Backtrace::obtain_backtrace(const bool use_expensive) {
#ifndef KRIMS_HAVE_GLIBC_STACKTRACE
  // If we do not have a GLIBC stacktrace, there is no reason to do anything
  // here.
  m_parsing_done = true;
  m_parsed_frames.clear();
  m_n_raw_frames = 0;

  // Fake-use parameter
  (void)use_expensive;
  return;
#else
  // If the system supports it, get a stacktrace how we got here
  // We defer the symbol lookup via backtrace_symbols() since
  // this loads external libraries which can take up to seconds
  // on some machines.
  // See generate_stacktrace() for the place where this is done.
  m_n_raw_frames = backtrace(m_raw_backtrace, n_max_frames);

#ifdef KRIMS_ADDR2LINE_AVAILABLE
  // We have addr2line, hence we will make use of it if the user wants expensive
  // methods to be used when parsing the stacktrace.
  m_determine_file_line = use_expensive;
#else
  // Fake-use parameter
  (void)use_expensive;
#endif

  m_parsed_frames.clear();
  m_parsing_done = false;
#endif
}

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
char* Backtrace::demangle(const char* mangled_name) const {
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
  // try to demangle the function name:
  int status;
  char* p = abi::__cxa_demangle(mangled_name, 0, 0, &status);

  if (status == 0) {
    return p;
  } else {
    return NULL;
  }
#else
  char* ret = static_cast<char*>(malloc(strlen(mangled_name)));
  if (ret == NULL) return NULL;
  strcpy(ret, mangled_name);
  return ret;
#endif
}

void Backtrace::split_backtrace_string(const char* symbol, Frame& frame) const {
  // The stacktrace frames are in the format
  //     executable(functionname+offset) [address]
  // Try to extract the functionname and the address
  const char* pos_bracketop = strchr(symbol, '(');
  const char* pos_bracketcl = strchr(symbol, ')');
  const char* pos_plus = strchr(symbol, '+');
  const char* pos_sqbrop = strchr(symbol, '[');
  const char* pos_sqbrcl = strchr(symbol, ']');

  // Extract the address:
  if (pos_sqbrop != NULL && pos_sqbrcl != NULL && pos_sqbrop < pos_sqbrcl) {
    const size_t len = pos_sqbrcl - pos_sqbrop - 1;
    frame.address = std::string(pos_sqbrop + 1, len);
  }

  // Extract the executable path
  if (pos_bracketop != NULL) {
    const size_t len = pos_bracketop - symbol;
    frame.executable_name = std::string(symbol, len);
  } else if (pos_sqbrop != NULL) {
    // There is a space after the executable name
    // and before the opening [
    const size_t len = pos_sqbrop - symbol - 1;
    frame.executable_name = std::string(symbol, len);
  }

  // Check whether functionname+offset is the empty string
  // or none of "(" or ")" exists:
  if (pos_bracketop == NULL || pos_bracketcl == NULL ||
      pos_bracketcl == pos_bracketop + 1) {
    // In this case -rdynamic has been forgotten and hence we
    // cannot lookup the function name symbol.
    frame.function_name = "? (add flag \"-rdynamic\" on linking)";
    return;
  }

  // Extract the function name:
  if (pos_bracketop != NULL && pos_plus != NULL && pos_bracketop < pos_plus) {
    const size_t len = pos_plus - pos_bracketop - 1;
    frame.function_name = std::string(pos_bracketop + 1, len);

    // Try to demangle the function name:
    char* demangled = demangle(frame.function_name.c_str());
    if (demangled != NULL) {
      frame.function_name = std::string(demangled);

      // Free the storage allocated in the demangle function:
      free(demangled);
    }
  }
}

void Backtrace::determine_file_line(const char* executable_name,
                                    const char* address, Frame& frame) const {
#ifndef KRIMS_ADDR2LINE_AVAILABLE
  // Nothing we can do: addr2line is not available:
  return;
#else
  if (strcmp(executable_name, Frame::unknown.c_str()) == 0 ||
      strcmp(address, Frame::unknown.c_str()) == 0) {
    // One of the arguments is equal to unknown, so we cannot call addr2line on
    // them.
    return;
  }

  // Allocate memory for addr2line call:
  const size_t maxlen = 4096;
  char* codefile = new char[maxlen];
  char* number = new char[maxlen];

  // call and interpret:
  int ret =
        krims::addr2line(executable_name, address, maxlen, codefile, number);

  if (ret == 0) {
    // All well, set values
    frame.codefile = std::string(codefile);
    frame.line_number = std::string(number);
  }

  // Free memory
  delete[] number;
  delete[] codefile;
#endif
}
#endif

void Backtrace::parse_backtrace() const {
  // If parsing was already done or there are no frames, return
  if (m_parsing_done) return;
  if (m_n_raw_frames <= 0) return;

  // Now we parse it all:
  m_parsing_done = true;

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
  // We have deferred the symbol lookup to this point to avoid costly
  // runtime penalties due to linkage of external libraries by
  // backtrace_symbols.
  char** stacktrace = backtrace_symbols(m_raw_backtrace, m_n_raw_frames);

  // TODO make this more configurable
  // Skip the frames we are not interested in. We test each frame from the
  // top (i.e. the outermost frame the call stack, where the main is located)
  // and move inwards until we find some frames which indicate internal stuff
  // from krims which gets called when processing an exception.
  int initframe = 0;
  for (int frame = m_n_raw_frames - 1; frame >= 0; --frame) {
    if (std::strstr(stacktrace[frame], "krims") &&
        std::strstr(stacktrace[frame], "ExceptionBase") &&
        std::strstr(stacktrace[frame], "add_exc_data")) {
      // The current call frame is responsible for adding the exception data
      // from the assert macros. so we are interested in the next one
      // (i.e. the one closer to main)
      initframe = frame + 1;
      break;
    } else if (std::strstr(stacktrace[frame], "krims") &&
               std::strstr(stacktrace[frame], "Backtrace") &&
               std::strstr(stacktrace[frame], "obtain_backtrace")) {
      // If the above is not triggered, but we find this current frame,
      // then a different mechanism was used than the assert macros.
      // As a fallback we start displaying from the next frame (i.e.
      // the one closer to main)
      initframe = frame + 1;
      break;
    }
  }

  for (int raw_i = initframe; raw_i < m_n_raw_frames; ++raw_i) {
    // Generate a new frame:
    m_parsed_frames.push_back(Frame{});
    Frame& frame = m_parsed_frames.back();

    split_backtrace_string(stacktrace[raw_i], frame);
    if (m_determine_file_line) {
      determine_file_line(frame.executable_name.c_str(), frame.address.c_str(),
                          frame);
    }

    // Stop processing the frames once we are in main():
    if (frame.function_name == "main") break;
  }

  // Free the memory glibc malloced for the stacktrace:
  free(stacktrace);
#endif
}

}  // namespace krims
