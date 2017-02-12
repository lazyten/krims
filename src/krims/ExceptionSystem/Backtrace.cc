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

#include "Backtrace.hh"
#include "krims/demangle.hh"
#include <iomanip>
#include <sstream>

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
#include <execinfo.h>
#endif

#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
#include <cxxabi.h>
#endif

namespace krims {

const std::string Backtrace::Frame::unknown = "?";

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
Backtrace::Backtrace() : m_raw_backtrace{}, m_parsed_frames{} {
  m_parsed_frames.reserve(n_max_frames);
}
#else
Backtrace::Backtrace() : m_parsed_frames{} {}
#endif  // KRIMS_HAVE_GLIBC_STACKTRACE

void Backtrace::obtain_backtrace(const bool use_expensive) {
  // Clear the parsed frames if any:
  m_parsed_frames.clear();

#ifndef KRIMS_HAVE_GLIBC_STACKTRACE
  m_n_raw_frames = 0;

  // Fake-use parameter and return
  (void)use_expensive;
  return;
#else  // not KRIMS_HAVE_GLIBC_STACKTRACE
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
#endif  // KRIMS_ADDR2LINE_AVAILABLE
#endif  // not KRIMS_HAVE_GLIBC_STACKTRACE

  parse_backtrace();
}

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
void Backtrace::split_backtrace_string(const char* symbol, Frame& frame) const {
  // The stacktrace frames are in the format
  //     executable(functionname+offset) [address]
  // Try to extract the functionname and the address
  const char* pos_bracketop = strchr(symbol, '(');
  const char* start = pos_bracketop == nullptr ? symbol : pos_bracketop;

  const char* pos_bracketcl = strchr(start, ')');
  const char* pos_plus = strchr(start, '+');
  const char* pos_sqbrop = strchr(start, '[');
  const char* pos_sqbrcl = strchr(start, ']');

  // Extract the address:
  if (pos_sqbrop != nullptr && pos_sqbrcl != nullptr && pos_sqbrop < pos_sqbrcl - 1) {
    const size_t len = static_cast<size_t>(pos_sqbrcl - pos_sqbrop - 1);
    frame.address = std::string(pos_sqbrop + 1, len);
  }

  // Extract the executable path
  if (pos_bracketop != nullptr && symbol < pos_bracketop) {
    const size_t len = static_cast<size_t>(pos_bracketop - symbol);
    frame.executable_name = std::string(symbol, len);
  } else if (pos_sqbrop != nullptr && symbol < pos_sqbrop - 1) {
    // There is a space after the executable name
    // and before the opening [
    const size_t len = static_cast<size_t>(pos_sqbrop - symbol - 1);
    frame.executable_name = std::string(symbol, len);
  }

  // Check whether functionname+offset is the empty string
  // or none of "(" or ")" exists:
  if (pos_bracketop == nullptr || pos_bracketcl == nullptr ||
      pos_bracketcl == pos_bracketop + 1) {
    // In this case -rdynamic has been forgotten and hence we
    // cannot lookup the function name symbol.
    frame.function_name = R"(? (add flag "-rdynamic" on linking))";
    return;
  }

  // Extract the function name:
  if (pos_bracketop != nullptr && pos_plus != nullptr && pos_bracketop < pos_plus) {
    const size_t len = static_cast<size_t>(pos_plus - pos_bracketop - 1);
    frame.function_name = std::string(pos_bracketop + 1, len);
    frame.function_name = demangled_string(frame.function_name);
  }
}

void Backtrace::determine_file_line(const char* executable_name, const char* address,
                                    Frame& frame) const {
#ifdef KRIMS_ADDR2LINE_AVAILABLE
  if (strcmp(executable_name, Frame::unknown.c_str()) == 0 ||
      strcmp(address, Frame::unknown.c_str()) == 0) {
    // One of the arguments is equal to unknown, so we cannot call addr2line on
    // them.
    return;
  }

  // Allocate memory for addr2line call:
  const size_t maxlen = 4096;
  auto* codefile = new char[maxlen];
  auto* number = new char[maxlen];

  // call and interpret:
  int ret = krims::addr2line(executable_name, address, maxlen, codefile, number);

  if (ret == 0) {
    // All well, set values
    frame.codefile = std::string(codefile);
    frame.line_number = std::string(number);
  }

  // Free memory
  delete[] number;
  delete[] codefile;
#endif  // KRIMS_ADDR2LINE_AVAILABLE
}
#endif  // KRIMS_HAVE_GLIBC_STACKTRACE

void Backtrace::parse_backtrace() {
  // If parsing was already done or there are no frames, return
  if (m_n_raw_frames <= 0) return;

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
    if ((std::strstr(stacktrace[frame], "krims") != nullptr) &&
        (std::strstr(stacktrace[frame], "detail") != nullptr) &&
        (std::strstr(stacktrace[frame], "throw_by_value") != nullptr)) {
      // The current call frame is responsible for adding the exception data
      // from the assert macros. so we are interested in the next one
      // (i.e. the one closer to main)
      initframe = frame + 1;
      break;
    }
    if (std::strstr(stacktrace[frame], "__cxa_call_unexpected") != nullptr) {
      // The current frame indicates that an unexpected exception was
      // caught. This is certainly already inside the exception handling
      // part of the c++ library. So we set the init frame to the next
      // frame, but we do not break in case we find something better.
      initframe = frame + 1;
    }
  }

  for (int raw_i = initframe; raw_i < m_n_raw_frames; ++raw_i) {
    // Generate a new frame:
    m_parsed_frames.emplace_back();
    Frame& frame = m_parsed_frames.back();

    split_backtrace_string(stacktrace[raw_i], frame);
    if (m_determine_file_line) {
      determine_file_line(frame.executable_name.c_str(), frame.address.c_str(), frame);
    }

    // Stop processing the frames once we are in main():
    if (frame.function_name == "main") break;
  }

  // Free the memory glibc malloced for the stacktrace:
  free(stacktrace);
#endif
}

std::ostream& operator<<(std::ostream& out, const Backtrace& bt) {
  // If we have no backtrace, print nothing.
  if (bt.frames().empty()) {
    std::cerr << "Sorry, no backtrace available" << std::endl;
    return out;
  }

  // Determine length of longest function name:
  size_t maxfunclen = 8;  // length of string "function"
  for (const auto& frame : bt.frames()) {
    maxfunclen = std::max(maxfunclen, frame.function_name.length());
  }

  // If the Function is longer than 80 columns than just print them as is
  // (otherwise we get too much empty space)
  if (maxfunclen > 80) maxfunclen = 8;
  const int maxlen = static_cast<int>(maxfunclen);

  // TODO Better table format
  //      Colour!

  // Print heading of the backtrace table:
  out << "## " << std::setw(maxlen) << std::left << "function" << std::right << " @ ";
  if (bt.determine_file_line()) {
    out << "    file    :  linenr" << std::endl;
  } else {
    out << " executable :  address" << std::endl;
  }
  out << "--------------------------------------" << std::endl << std::endl;

  for (size_t i = 0; i < bt.frames().size(); ++i) {
    const Backtrace::Frame& frame = bt.frames()[i];

    out << std::setw(2) << i << " " << std::setw(maxlen) << std::left
        << frame.function_name << std::right << " @ ";

    // Was the determine_file_line call to addr2line successful?
    const bool file_line_successful =
          !frame.codefile.empty() && frame.codefile[0] != '?' &&
          !frame.line_number.empty() && frame.line_number[0] != '?';

    if (bt.determine_file_line() && file_line_successful) {
      out << frame.codefile << "  :  " << frame.line_number;
    } else {
      out << frame.executable_name << "  :  " << frame.address;
    }
    out << std::endl;
  }

  if (!bt.determine_file_line()) {
    out << std::endl
        << R"(Hint: Use "addr2line -e <executable> <address>" to get file and line number in backtrace.)"
        << std::endl;
  }

  return out;
}

}  // namespace krims
