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

ParameterMap& ParameterMap::operator=(ParameterMap other) {
  m_location = std::move(other.m_location);
  m_container_ptr = std::move(other.m_container_ptr);
  return *this;
}

ParameterMap::ParameterMap(const ParameterMap& other) : ParameterMap() {
  if (other.m_location == std::string("")) {
    // We are root, copy everything
    m_container_ptr = std::make_shared<inner_map_type>(*other.m_container_ptr);
  } else {
    update(other);
  }
}

void ParameterMap::update(std::initializer_list<entry_type> il) {
  // Make each key a full path key and append/modify entry in map
  for (entry_type t : il) {
    (*m_container_ptr)[make_full_key(t.first)] = std::move(t.second);
  }
}

void ParameterMap::update(const std::string& key, const ParameterMap& other) {
  for (auto it = other.begin_keys(); it != other.end_keys(); ++it) {
    // Truncate the other key, relative to its location, then
    // make it full for our location and update
    std::string relloc = other.strip_location_prefix(it.m_iter->first);
    (*m_container_ptr)[make_full_key(key + "/" + relloc)] = it.m_iter->second;
  }
}

void ParameterMap::update(const std::string& key, ParameterMap&& other) {
  for (auto it = other.begin_keys(); it != other.end_keys(); ++it) {
    // Truncate the other key, relative to its location, then
    // make it full for our location and update
    std::string relloc = other.strip_location_prefix(it.m_iter->first);
    (*m_container_ptr)[make_full_key(key + "/" + relloc)] = std::move(it.m_iter->second);
  }
}

std::string ParameterMap::make_full_key(const std::string& key) const {
  assert_dbg(m_location[0] == '/' || m_location.length() == 0, krims::ExcInternalError());
  assert_dbg(m_location.back() != '/', krims::ExcInternalError());

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

    if (part == ".") {
      // Ignore "." path part (does nothing)
      continue;
    } else if (part == "..") {
      // If ".." path part, then pop the most recently added path part if any.
      if (pathparts.size() != 0) pathparts.pop_back();
    } else {
      pathparts.push_back(std::move(part));
    }
  }

  std::string res{m_location};
  for (const auto& part : pathparts) {
    res += "/" + part;
  }

  assert_dbg(res.back() != '/', krims::ExcInternalError());
  assert_dbg(res[0] == '/' || res.length() == 0, krims::ExcInternalError());

  return res;
}

std::string ParameterMap::strip_location_prefix(const std::string& key) const {
  // The first part needs to be exactly the location:
  assert_dbg(0 == key.compare(0, m_location.size(), m_location), ExcInternalError());

  if (key.size() <= m_location.size()) {
    // Cannot remove something
    return "";
  } else {
    std::string res = key.substr(m_location.size());
    assert_dbg(res[0] == '/' || res.length() == 0, ExcInternalError());
    assert_dbg(res.back() != '/', ExcInternalError());
    return res;
  }
}

ParameterMap::KeyIterator ParameterMap::begin_keys() const {
  // obtain lower bound to the root location
  // (since the keys are sorted alphabetically in the map
  //  the ones which follow next must all be below our current
  //  location)

  auto it = m_container_ptr->lower_bound(m_location);
  return KeyIterator(std::move(it), *this);
}

ParameterMap::KeyIterator ParameterMap::end_keys() const {
  // We call begin_keys and advance until we find the first key
  // which is no longer part of this subtree (i.e. starts differently)
  auto it = begin_keys();
  for (; it.m_iter != std::end(*m_container_ptr); ++it) {
    if (0 != it.m_iter->first.compare(0, m_location.length(), m_location)) break;
  }
  return it;
}
}  // namespace krims
