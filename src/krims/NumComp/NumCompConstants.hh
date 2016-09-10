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

namespace krims {

/** What action to undertake in case a comparison fails:
 * Return false, throw an NumCompException or throw a verbose
 * NumCompException */
enum class NumCompActionType { Return, ThrowNormal, ThrowVerbose };

/** A flag to increase or decrease the tolerance based on the default
 *  value in order to change the accuracy for which the numerical
 *  comparison checks.
 *
 * MachinePrecision => std::numeric_limits<error_type>::epsilon()
 * TenMachinePrecision => 10*std::numeric_limits<error_type>::epsilon()
 * Extreme     => tolerance_factor /  100 * MachinePrecision
 * Higher      => tolerance_factor /   10 * MachinePrecision
 * Default     => tolerance_factor        * MachinePrecision
 * Lower       => tolerance_factor *   10 * MachinePrecision
 * Sloppy      => tolerance_factor *  100 * MachinePrecision
 * SuperSloppy => tolerance_factor * 1000 * MachinePrecision
 *
 * where tolerance_factor is NumCompConstants::default_tolerance_factor
 **/
enum class NumCompAccuracyLevel {
  /** equals std::numeric_limits<error_type>::epsilon() */
  MachinePrecision,

  /** equals 10*std::numeric_limits<error_type>::epsilon() */
  TenMachinePrecision,

  /** equals tolerance_factor /  100 * MachinePrecision */
  Extreme,

  /** equals tolerance_factor /   10 * MachinePrecision */
  Higher,

  /** equals tolerance_factor        * MachinePrecision */
  Default,

  /** equals tolerance_factor *   10 * MachinePrecision */
  Lower,

  /** equals tolerance_factor *  100 * MachinePrecision */
  Sloppy,

  /** equals tolerance_factor * 1000 * MachinePrecision */
  SuperSloppy,
};

/** A struct which holds static constants influencing the default behaviour of
 * NumComp */
struct NumCompConstants {
  /** The default tolerance_factor to use
   *
   * \see NumCompAccuracyLevel for more details.
   * */
  static double default_tolerance_factor;
  // TODO hard-coded double type

  /** The default failure action */
  static NumCompActionType default_failure_action;

  /** An inner struct to temporarily change the NumCompConstants.
   *  Once the class goes out of scope and is destroyed, the original
   *  settings are restored */
  struct CacheChange {
  public:
    /** Change both the default failure action as well as the tolerance factor
     * until the class goes out of scope*/
    CacheChange(double tolerance_factor, NumCompActionType failure_action)
          : m_do_restore{true},
            m_orig_default_tolerance_factor(default_tolerance_factor),
            m_orig_default_failure_action(default_failure_action) {
      default_tolerance_factor = tolerance_factor;
      default_failure_action = failure_action;
    }

    /** Restore original settings */
    ~CacheChange() {
      if (!m_do_restore) return;
      default_failure_action = m_orig_default_failure_action;
      default_tolerance_factor = m_orig_default_tolerance_factor;
    }

    // Disallow copying the class around:
    CacheChange(const CacheChange&) = delete;
    CacheChange& operator=(const CacheChange&) = delete;

    // Moving is allowed, though:
    CacheChange(CacheChange&& other)
          : m_do_restore{other.m_do_restore},
            m_orig_default_tolerance_factor{
                  other.m_orig_default_tolerance_factor},
            m_orig_default_failure_action{other.m_orig_default_failure_action} {
      other.m_do_restore = false;
    }

    CacheChange& operator=(CacheChange&& other) {
      m_do_restore = other.m_do_restore;
      m_orig_default_failure_action = other.m_orig_default_failure_action;
      m_orig_default_tolerance_factor = other.m_orig_default_tolerance_factor;
      other.m_do_restore = false;
      return *this;
    }

  private:
    bool m_do_restore = true;
    double m_orig_default_tolerance_factor;
    NumCompActionType m_orig_default_failure_action;
  };  // class CacheChange

  /** Temporarily change the tolerance factor until the guarding CacheChange
   * class goes out of scope.
   *  Do not use this function to permanently change the default tolerance
   * factor. For this use the static members above directly.
   */
  static CacheChange change_temporary(
        double tolerance_factor,
        NumCompActionType failure_action = default_failure_action) {
    return CacheChange{tolerance_factor, failure_action};
  }

  /** Temporarily change the failure action until the guarding CacheChange
   * class goes out of scope.
   *  Do not use this function to permanently change the default failure action.
   * For this use the static members above directly.
   */
  static CacheChange change_temporary(NumCompActionType failure_action) {
    return CacheChange{default_tolerance_factor, failure_action};
  }
};  // class NumCompConstants

}  // namespace krims
