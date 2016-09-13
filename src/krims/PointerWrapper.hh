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
#include "Subscribable.hh"
#include "SubscriptionPointer.hh"
#include <type_traits>

namespace krims {

/** \brief Wrapper class taking either a std::shared_ptr or a subscription
 * pointer.
 *
 * This class can take either a shared_ptr or if this is supported by T (i.e. if
 * T is derived off krims::Subscribable) it can also take a
 * SubscriptionPointer<T>.
 *
 * The idea is to have a class that transparently behaves like a basic pointer
 * indifferent to the actual contained pointer, but where a (more powerful)
 * std::shared_ptr can still be extracted from.
 *
 * The primary template defined here may only take a shared pointer.
 **/
template <typename T, typename = void>
class PointerWrapper {
  // Implementation for std::false_type, i.e. we don't have a subscribable
public:
  /** Construct PointerWrapper from shared pointer */
  PointerWrapper(const std::shared_ptr<T> ptr) : m_shared_ptr{std::move(ptr)} {}

  /** \brief Check if this object is empty or not */
  explicit operator bool() const { return m_shared_ptr != nullptr; }

  /** \brief Raw access to the inner pointer */
  T* get() const { return m_shared_ptr.get(); }

  /** Dereference object */
  T& operator*() const {
    assert_dbg(get() != nullptr, ExcInvalidPointer());
    return *m_shared_ptr.get();
  }

  /** Dereference object member */
  T* operator->() const {
    assert_dbg(get() != nullptr, ExcInvalidPointer());
    return m_shared_ptr.get();
  }

  /** Make a shared_ptr out of the PointerWrapper
   *
   * \note This version might be called implicitly, which does not
   * hurt since this guy effectively is a shared_ptr.
   * */
  operator std::shared_ptr<T>() const { return m_shared_ptr; }

  /** Does this wrapper contain a shared pointer (true) or a
   *  subscription pointer (false)
   **/
  constexpr bool is_shared_ptr() const { return true; }

private:
  const std::shared_ptr<T> m_shared_ptr;
};

/** \brief Wrapper class taking either a std::shared_ptr or a subscription
 * pointer.
 *
 * For more details see the primary template. This partial specialisation
 * may take both a std::shared_ptr as well as a SubscriptionPointer<T>.
 **/
template <typename T>
class PointerWrapper<T, typename std::enable_if<
                              std::is_base_of<Subscribable, T>::value>::type> {
  // Implementation for std::true_type, i.e. we have a subscribable T
public:
  /** Construct PointerWrapper from subscription pointer */
  PointerWrapper(const SubscriptionPointer<T> ptr)
        : m_subscr_ptr{std::move(ptr)},
          m_shared_ptr{nullptr},
          m_contains_shared_ptr{false} {}

  /** Construct PointerWrapper from shared pointer */
  PointerWrapper(const std::shared_ptr<T> ptr)
        : m_subscr_ptr{"PointerWrapper"},
          m_shared_ptr{std::move(ptr)},
          m_contains_shared_ptr{true} {}

  /** \brief Check if this object is empty or not */
  explicit operator bool() const { return get() != nullptr; }

  /** \brief Raw access to the inner pointer */
  T* get() const {
    if (m_contains_shared_ptr) return m_shared_ptr.get();
    return m_subscr_ptr.get();
  }

  /** Dereference object */
  T& operator*() const {
    assert_dbg(get() != nullptr, ExcInvalidPointer());
    return *get();
  }

  /** Dereference object member */
  T* operator->() const {
    assert_dbg(get() != nullptr, ExcInvalidPointer());
    return get();
  }

  /** Explicitly make a shared_ptr out of the PointerWrapper
   *
   * \note This operation is only allowed in Release mode or if this wrapper
   *       actually contains a shared pointer.*/
  explicit operator std::shared_ptr<T>() const {
    if (m_contains_shared_ptr) {
      return m_shared_ptr;
    } else if (m_subscr_ptr == nullptr) {
      return std::shared_ptr<T>{};
    } else {
      assert_dbg(
            false,
            ExcDisabled(
                  "Casting a PointerWrapper to a shared pointer which does not "
                  "contain a shared ptr internally implies a copying of the "
                  "full data and is hence disabled. Perform an explicit copy "
                  "instead by dereferencing the result of the get() function "
                  "and employing it together with std::make_shared."));
      return std::make_shared<T>(*m_subscr_ptr);
    }
  }

  /** Make a subscription pointer out of the PointerWrapper
   *
   * \note This operation may be called implicitly, which does not hurt
   * since T is a subscribable type anyways.
   */
  operator SubscriptionPointer<T>() const {
    if (!m_contains_shared_ptr) {
      // we contain a subscription pointer, so return a
      // copy of it
      return m_subscr_ptr;
    } else if (m_shared_ptr == nullptr) {
      // We contain no valid shared pointer either
      // ... return subscription to nullptr:
      return SubscriptionPointer<T>("PointerWrapper");
    } else {
      // Here we have a valid object to point to
      // Subscribe to it:
      return SubscriptionPointer<T>("PointerWrapper", *m_shared_ptr);
    }
  }

  /** Does this wrapper contain a shared pointer (true) or a
   *  subscription pointer (false)
   *
   * \note In case it contains a shared pointer, casting this pointer to a
   *       shared pointer is a constant operation, otherwise it will abort
   *       the program in Debug mode, but will proceed in Release mode and
   *       thereby copy the contained data.
   **/
  bool is_shared_ptr() const { return m_contains_shared_ptr; }

private:
  //! The stored subscription pointer (or a nullptr)
  const SubscriptionPointer<T> m_subscr_ptr;

  //! The stored shared pointer (or a nullptr)
  const std::shared_ptr<T> m_shared_ptr;

  //! Does this class contain a shared pointer?
  const bool m_contains_shared_ptr;
};

//
// == and != comparison operators:
//
/** Compare if the PointerWrapper point to the same object */
template <typename T>
inline bool operator==(const PointerWrapper<T>& lhs,
                       const PointerWrapper<T>& rhs) {
  return lhs.get() == rhs.get();
}

/** Compare if the PointerWrappers do not point to the same object */
template <typename T>
inline bool operator!=(const PointerWrapper<T>& lhs,
                       const PointerWrapper<T>& rhs) {
  return !operator==(lhs, rhs);
}

/** Compare if the PointerWrapper points to no object, i.e. stores a
 * nullptr */
template <typename T>
inline bool operator==(const PointerWrapper<T>& lhs, std::nullptr_t) {
  return lhs.get() == nullptr;
}

/** Compare if the PointerWrapper pointer points to no object, i.e. stores a
 * nullptr */
template <typename T>
inline bool operator==(std::nullptr_t, const PointerWrapper<T>& rhs) {
  return rhs == nullptr;
}

/** Compare if the PointerWrapper pointer does not point to no object, i.e.
 * stores no nullptr */
template <typename T>
inline bool operator!=(const PointerWrapper<T>& lhs, std::nullptr_t) {
  return !operator==(lhs, nullptr);
}

/** Compare if the PointerWrapper pointer does not point to no object, i.e.
 * stores no nullptr */
template <typename T>
inline bool operator!=(std::nullptr_t, const PointerWrapper<T>& rhs) {
  return !operator==(nullptr, rhs);
}

}  // namespace krims
