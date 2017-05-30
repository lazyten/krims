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

#include "GenMap.hh"
#include <vector>

namespace krims {

GenMap& GenMap::operator=(GenMap other) {
  m_location = std::move(other.m_location);
  m_container_ptr = std::move(other.m_container_ptr);
  return *this;
}

GenMap::GenMap(const GenMap& other) : GenMap() {
  if (other.m_location == std::string("")) {
    // We are root, copy everything
    m_container_ptr = std::make_shared<map_type>(*other.m_container_ptr);
  } else {
    update(other);
  }
}

void GenMap::update(std::initializer_list<entry_type> il) {
  // Make each key a full path key and append/modify entry in map
  for (entry_type t : il) {
    (*m_container_ptr)[make_full_key(t.first)] = std::move(t.second);
  }
}

void GenMap::clear() {
  if (m_location == std::string("")) {
    // We are root, clear everything
    m_container_ptr->clear();
  } else {
    // Clear only our stuff
    erase(begin(), end());
  }
}

void GenMap::update(const std::string& key, const GenMap& other) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    // The iterator truncates the other key relative to the builtin
    // location of other for us. We then make it full for our location
    // and update.
    (*m_container_ptr)[make_full_key(key + "/" + it->key())] = it->value_raw();
  }
}

void GenMap::update(const std::string& key, GenMap&& other) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    // The iterator truncates the other key relative to the builtin
    // location of other for us. We then make it full for our location
    // and update.
    (*m_container_ptr)[make_full_key(key + "/" + it->key())] = std::move(it->value_raw());
  }
}

std::string GenMap::make_full_key(const std::string& key) const {
  assert_internal(m_location[0] == '/' || m_location.length() == 0);
  assert_internal(m_location.back() != '/');

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
    std::string part = key.substr(start, end - start);

    // Update start for next iteration:
    start += part.length();

    if (part == ".") {
      // Ignore "." path part (does nothing)
      continue;
    } else if (part == "..") {
      // If ".." path part, then pop the most recently added path part if any.
      if (!pathparts.empty()) pathparts.pop_back();
    } else {
      pathparts.push_back(std::move(part));
    }
  }

  std::string res{m_location};
  for (const auto& part : pathparts) {
    res += "/" + part;
  }

  assert_internal(res.length() == 0 || res.back() != '/');
  assert_internal(res.length() == 0 || res[0] == '/');

  return res;
}

typename GenMap::iterator GenMap::begin(const std::string& path) {
  const std::string path_full = make_full_key(path);

  // Obtain iterator to the first key-value pair, which has a
  // key starting with the full path.
  //
  // (since the keys are sorted alphabetically in the map
  //  the ones which follow next must all be below our current
  //  location or already well past it.)
  auto it = starting_keys_begin(*m_container_ptr, path_full);
  return iterator(it, path_full);
}

typename GenMap::const_iterator GenMap::cbegin(const std::string& path) const {
  const std::string path_full = make_full_key(path);
  auto it = starting_keys_begin(*m_container_ptr, path_full);
  return const_iterator(it, path_full);
}

typename GenMap::iterator GenMap::end(const std::string& path) {
  const std::string path_full = make_full_key(path);

  // Obtain the first key which does no longer start with the pull path,
  // i.e. where we are done processing the subpath.
  auto it = starting_keys_end(*m_container_ptr, path_full);
  return iterator(it, path_full);
}

typename GenMap::const_iterator GenMap::cend(const std::string& path) const {
  const std::string path_full = make_full_key(path);
  auto it = starting_keys_end(*m_container_ptr, path_full);
  return const_iterator(it, path_full);
}

}  // namespace krims
