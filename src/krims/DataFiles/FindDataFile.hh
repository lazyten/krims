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
#include "krims/ExceptionSystem.hh"
#include <array>
#include <vector>

namespace krims {

DefException3(
      ExcDatafileNotFound, std::string, std::string, std::string,
      << "Could not find the static data file \"" << arg1
      << "\" in the static data directories. Tried directories:\n"
      << arg2 << "\n"
      << "You can add further directories to this list using the environment variables "
      << arg3 << ".\n"
      << "See the documentation for more details.");

/** Class to change the order in which the individual find steps are executed */
enum class FindStep {
  /** Search the paths provided by environment variables
   *  and the paths derived from these by adding suffixes */
  Environment,

  /** Search the explicit directories given in a list */
  ExtraDirectories,

  /** Search the working directory and/or directories derived
   *  from the working directory. */
  WorkingDirectory
};

/** Functor to aid with finding static data files */
struct FindDataFile {
  /** Find a file according to the criteria given in this
   *  class and return it */
  std::string operator()(const std::string& file) const;

  /** The environment variables considered. These values will be read and the suffixes
   *  given in env_suffixes will be added */
  std::vector<std::string> env_vars;

  /** Suffixes added when searching environment variables */
  std::vector<std::string> env_suffixes;

  /** Explicit directories to be searched */
  std::vector<std::string> extra_directories;

  /** Suffixes added to the current working directory */
  std::vector<std::string> cwd_suffixes;

  /** The list of find steps executed and the order in which this is done */
  std::array<FindStep, 3> find_steps{
        {FindStep::Environment, FindStep::WorkingDirectory, FindStep::ExtraDirectories}};

  /** Return the list of all directories which will be searched,
   * exactly in the order they will be searched in the current
   * configuration */
  std::vector<std::string> search_directories() const;

  /** Default constructor
   *
   * Searches for the data file:
   *   - Path of environment variable DATA_DIR
   *   - the subdirectory data of the current working directory
   */
  FindDataFile()
        : env_vars{{"DATA_DIR"}},
          env_suffixes{{""}},
          extra_directories{},
          cwd_suffixes{"data"} {};

  /** Constructor with identifier
   *
   * Next to the default constructor this version also
   * implies searching in those paths:
   *    - The subdir `identifier` of DATA_DIR
   *    - The subdir `identifier` of IDENTIFIER_DATA_DIR
   *    - The dir represented by IDENTIFIER_DATA_DIR
   *    - The subdirectory data/identifier of the
   *      current working directory
   */
  FindDataFile(const std::string& identifier);

 private:
  /* Try to find it in one of the environment-provided paths.
   * Return empty string if not found */
  std::vector<std::string> searchdirs_environ() const;

  /** Try to find it in the current working directory */
  std::vector<std::string> searchdirs_cwd() const;
};

}  // namespace krims
