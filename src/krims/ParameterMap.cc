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

//
// Entry subclass
//
ParameterMap::Entry::Entry()
      : m_object_ptr(nullptr), m_via_subscription_ptr(false) {
#ifdef DEBUG
  m_type_name = "";
#endif
}

//
// ParameterMap
//

/** Remove an element */
void ParameterMap::erase(const std::string& key) {
  assert_dbg(exists(key), ExcUnknownKey(key));
  m_container.erase(key);
}

/** Check weather a key exists */
bool ParameterMap::exists(const std::string& key) const {
  return m_container.find(key) != std::end(m_container);
}

}  // krims
