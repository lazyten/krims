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

#include "FindDataFile.hh"
#include "krims/Algorithm.hh"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iterator>
#include <string>
#include <unistd.h>

namespace krims {

std::string FindDataFile::operator()(const std::string& file) const {
  // TODO Multiplex for C++17 filesystem stuff
  auto file_exists = [](const std::string& f) { return access(f.c_str(), R_OK) != -1; };
  auto realpath = [](const std::string& p) {
    char* rp = ::realpath(p.c_str(), nullptr);
    return std::string(rp);
  };

  // If we already have a full path, we are done:
  if (file_exists(file)) return realpath(file);

  const std::vector<std::string> searchdirs = search_directories();
  for (const auto& dir : searchdirs) {
    std::string fullpath(dir + "/" + file);
    if (file_exists(fullpath)) return realpath(fullpath);
  }

  // File not found => tell user where we looked:
  const std::string pathstring =
        std::string("\"")
              .append(join(searchdirs.begin(), searchdirs.end(), "\", \""))
              .append("\"");
  const std::string envstring =
        std::string("\"")
              .append(join(env_vars.begin(), env_vars.end(), "\", \""))
              .append("\"");

  assert_throw(false, ExcDatafileNotFound(file, pathstring, envstring));
  return "<file not found>";
}

std::vector<std::string> FindDataFile::search_directories() const {
  std::vector<std::string> ret;

  for (FindStep s : find_steps) {
    // Find dirs for this step:
    std::vector<std::string> s_dirs = [&s, this]() {
      switch (s) {
        case FindStep::Environment:
          return searchdirs_environ();
        case FindStep::ExtraDirectories:
          return extra_directories;
        case FindStep::WorkingDirectory:
          return searchdirs_cwd();
      }
    }();

    // Insert into ret:
    std::back_insert_iterator<decltype(ret)> backit(ret);
    std::move(std::begin(s_dirs), std::end(s_dirs), backit);
  }

  return ret;
}

FindDataFile::FindDataFile(const std::string& identifier) : FindDataFile() {
  // Upper case version of identifier:
  const std::string id_upper = [](std::string str) {
    for (auto& c : str) c = static_cast<char>(toupper(c));
    return str;
  }(identifier);

  env_vars.insert(std::begin(env_vars), id_upper + "_DATA_DIR");
  env_suffixes.insert(std::begin(env_vars), identifier);
  cwd_suffixes.insert(std::begin(env_vars), "data/" + identifier);
}

std::vector<std::string> FindDataFile::searchdirs_environ() const {
  // Obtain the values of the environment variables:
  std::vector<const char*> env_vals(env_vars.size());
  std::transform(std::begin(env_vars), std::end(env_vars), std::begin(env_vals),
                 [](const std::string& s) { return getenv(s.c_str()); });

  // Compute full search paths and push back:
  std::vector<std::string> search;
  for (const char* path : env_vals) {
    if (path == nullptr) continue;
    for (const auto& suf : env_suffixes) {
      std::string fullpath(path);
      fullpath.append("/").append(suf);
      search.push_back(std::move(fullpath));
    }
  }
  return search;
}

std::vector<std::string> FindDataFile::searchdirs_cwd() const {
  // Get current working directory:
  char cwd[FILENAME_MAX];
  if (getcwd(cwd, sizeof(cwd)) == nullptr) return {};

  std::vector<std::string> search(cwd_suffixes.size());
  std::transform(std::begin(cwd_suffixes), std::end(cwd_suffixes), std::begin(search),
                 [&cwd](const std::string& suf) {
                   std::string s(cwd);
                   s.append("/").append(suf);
                   return s;
                 });
  return search;
}

}  // namespace krims
