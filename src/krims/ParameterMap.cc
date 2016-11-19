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

#include "ParameterMap.hh"

namespace krims {

ParameterMap& ParameterMap::operator=(const ParameterMap& other) {
  m_location = other.m_location;
  m_container_ptr = std::make_shared<inner_map_type>(*other.m_container_ptr);
  return *this;
}

ParameterMap ParameterMap::submap(const std::string& location) {
  // Clean the location path first:
  std::string newlocation = make_full_key(location);

  // Add a tailing "/" if not yet there
  if (newlocation.back() != '/') newlocation += "/";

  // Use a special constructor, which makes the resulting ParameterMap object
  // share the map string->EntryValue, but start at a different location
  return ParameterMap{m_container_ptr, newlocation};
}

const ParameterMap ParameterMap::submap(const std::string& location) const {
  // Normalise path:
  std::string newlocation = make_full_key(location);
  if (newlocation.back() != '/') newlocation += "/";

  // Construct new map, but starting at a different location
  return ParameterMap{m_container_ptr, newlocation};
}

std::string ParameterMap::make_full_key(const std::string& key) const {
  assert_dbg(m_location[0] == '/', krims::ExcInternalError());
  assert_dbg(m_location.back() == '/', krims::ExcInternalError());

  // Make a stack out of the key:
  std::vector<std::string> pathparts;

  // start gives the location after the last '/',
  // ie where the current part of the key path begins and end gives
  // the location of the current '/', i.e. the past-the-end index
  // of the current path part.
  for (size_t start = 0; start < key.size(); ++start) {
    // Past-the-end of the current path part:
    const size_t end = key.find('/', start);

    // Empty path part (i.e. something like '//' is encountered:
    if (start == end) continue;

    // Extract the part we deal with in this iteration:
    const std::string part = key.substr(start, end - start);

    // Update start for next iteration:
    start += part.length();

    // Ignore "." path part (does nothing)
    if (part == ".") continue;

    // If ".." path part, then pop the most recently added path part if any.
    if (part == "..") {
      if (pathparts.size() != 0) pathparts.pop_back();
    } else {
      pathparts.push_back(std::move(part));
    }
  }

  std::string res{m_location};
  for (const auto& part : pathparts) {
    res += part + "/";
  }

  assert_dbg(res.back() == '/', krims::ExcInternalError());

  // Remove the '/' at the end if res is not "/"
  if (res != "/") res.pop_back();

  return res;
}

}  // namespace krims
