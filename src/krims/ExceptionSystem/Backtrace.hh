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

#pragma once
#include "addr2line.hh"
#include <string>
#include <vector>

// TODO rewrite this class such that all memory which is used is allocated as
// soon as the class itself is allocated via the constructor and all other
// methods do only use said memory and none else.

namespace krims {

class Backtrace {
 public:
  // Struct to represent one frame in the Backtrace
  struct Frame {
    //! Static string to describe an unknown piece of information in the frame
    static const std::string unknown;

    /** The name of the executable of the current backtrace entry (or '?' if not
    known) **/
    std::string executable_name;

    /** The address of the backtrace symbol inside the exectuable (or '?' if not
    known) **/
    std::string address;

    /** The name of the function of the current backtrace entry (or '?' if not
    known) **/
    std::string function_name;

    //! The path to the file of the current backtrace entry (or '?' if not
    //! known)
    std::string codefile;

    //! The line number of the current backtrace entry (or '?' if not known)
    std::string line_number;

    //! Construct a frame.
    Frame()
          : executable_name{unknown},
            address{unknown},
            function_name{unknown},
            codefile{unknown},
            line_number{unknown} {}
  };

  /** The maximal number of backtrace frames we can obtain and use */
  static constexpr int n_max_frames = 25;

  /** Construct the class and allocate some memory. */
  Backtrace();

  /** Default destructor */
  ~Backtrace() = default;

  /** \brief Obtain a backtrace at the current position.
   *
   * \param use_expensive  Turn on more detailed, but expensive diagnostics (if
   *                       available). Examples are the determination of the
   *                       file path and line number for each backtrace entry.
   *                       The parsing of the backtrace data may take
   *                       considerably longer if this is set to true.
   **/
  void obtain_backtrace(const bool use_expensive = false);

  /** \brief Return the parsed backtrace frames */
  const std::vector<Frame>& frames() const {
    if (!m_parsing_done) parse_backtrace();
    return m_parsed_frames;
  }

  /** Return whether the parsing process has determined/will determine
   * the file path and the line numbers for each backtrace entry.
   */
  bool determine_file_line() const { return m_determine_file_line; }

 private:
#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
  /** Parse a string from backtrace_symbols representing a backtrace line
   *  into a Backtrace::Frame */
  void split_backtrace_string(const char* symbol, Frame& frame) const;

  /** Use the executable_name and the address data and call
   *  addr2line in order to determine the codefile and the linenumber.
   *  This is a rather expensive operation.
   */
  void determine_file_line(const char* executable_name, const char* address,
                           Frame& frame) const;
#endif

  /** Parse the m_raw_backtrace into m_parsed_entries */
  void parse_backtrace() const;

#ifdef KRIMS_HAVE_GLIBC_STACKTRACE
  /**
   *   array of pointers that contains the raw stack trace
   */
  void* m_raw_backtrace[n_max_frames]{};
#endif

  /** The actual number of raw backtrace frames we obtained in m_raw_backtrace.
   *  In case no frames can be obtained. This number is always zero.*/
  int m_n_raw_frames = 0;

  /** std::vector of Entry objects which contains the parsed stack trace. */
  mutable std::vector<Frame> m_parsed_frames;

  /** Flag to hold whether the parsing has been done */
  mutable bool m_parsing_done = false;

  /** Did the parsing process determine the file path and line numbers. */
  bool m_determine_file_line = false;
};

}  // namespace krims
