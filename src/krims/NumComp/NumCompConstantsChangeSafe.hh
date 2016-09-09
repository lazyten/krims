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

#pragma once
#include "NumCompConstants.hh"
namespace krims {

/** A struct to temporarily change the NumCompConstants.
 *  Once the class goes out of scope and is destroyed, the original
 *  settings are restored */
class NumCompConstantsChangeSafe {
private:
  NumCompConstantsChangeSafe()
        : m_orig_default_tolerance_factor(
                NumCompConstants::default_tolerance_factor),
          m_orig_default_failure_action(
                NumCompConstants::default_failure_action) {}

public:
  /** Change the tolerance factor until the class goes out of scope */
  NumCompConstantsChangeSafe(double tolerance_factor)
        : NumCompConstantsChangeSafe{} {
    NumCompConstants::default_tolerance_factor = tolerance_factor;
  }

  /** Change the default failure action until the class goes out of scope */
  NumCompConstantsChangeSafe(NumCompActionType failure_action)
        : NumCompConstantsChangeSafe{} {
    NumCompConstants::default_failure_action = failure_action;
  }

  /** Change both the default failure action as well as the tolerance factor
   * until the class goes out of scope*/
  NumCompConstantsChangeSafe(double tolerance_factor,
                             NumCompActionType failure_action)
        : NumCompConstantsChangeSafe{} {
    NumCompConstants::default_tolerance_factor = tolerance_factor;
    NumCompConstants::default_failure_action = failure_action;
  }

  ~NumCompConstantsChangeSafe() {
    NumCompConstants::default_failure_action = m_orig_default_failure_action;
    NumCompConstants::default_tolerance_factor =
          m_orig_default_tolerance_factor;
  }

  // Disallow moving or copying the class around:
  NumCompConstantsChangeSafe(const NumCompConstantsChangeSafe&) = delete;
  NumCompConstantsChangeSafe(NumCompConstantsChangeSafe&&) = delete;
  NumCompConstantsChangeSafe& operator=(NumCompConstantsChangeSafe&&) = delete;
  NumCompConstantsChangeSafe& operator=(const NumCompConstantsChangeSafe&) =
        delete;

private:
  double m_orig_default_tolerance_factor;
  NumCompActionType m_orig_default_failure_action;
};

}  // namespace krims
