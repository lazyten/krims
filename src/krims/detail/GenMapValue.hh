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
#include "krims/ExceptionSystem/Exceptions.hh"
#include "krims/RCPWrapper.hh"
#include "krims/SubscriptionPointer.hh"
#include "krims/TypeUtils.hh"
#include "krims/demangle.hh"

namespace krims {

// Forward-declare. Proper declaration in GenMap.hh
class GenMap;

namespace detail {

/** \brief Class to contain an entry value in a GenMap, i.e. the thing the
 *  key string actually points to.
 *
 * Can be constructed from elementary types by copying their value or from
 * a std::shared_ptr or a SubscriptionPointer to the object we emplace in the
 * map or from an object which is subscribable (which will automatically be
 * subscribed to.
 */
class GenMapValue {
 public:
  /** Exception to indicate that a wrong type was requested */
  DefException2(ExcWrongTypeRequested, std::string, std::string,
                << "Requested invalid type '" << arg1 << "' from GenMap."
                << " The value has type '" << arg2 << "'.");

  /** \brief Default constructor: Constructs empty object */
  GenMapValue() : m_object_ptr_ptr{nullptr} {}

  /** \brief Make a GenMapValue out of a type which is cheap to copy.
   *
   * This includes std::string and all relevant numeric types (integers,
   * floating point numbers, complex numbers)
   **/
  template <typename T, typename std::enable_if<!std::is_reference<T>::value &&
                                                      IsCheaplyCopyable<T>::value,
                                                int>::type = 0>
  GenMapValue(T t) {
    // Note about the enable_if:
    //   - We need to make sure that T is the actual type (and not a
    //     reference)
    //   - T should be cheap to copy
    copy_in<T>(t);
  }

  /** \brief Make an GenMapValue out of a const char*.
   *
   * This behaves like the equivalent GenMapValue of a  std::string */
  GenMapValue(const char* s) : GenMapValue(std::string(s)) {}

  /** \brief Make an GenMapValue from a shared pointer */
  template <typename T,
            typename = krims::enable_if_t<!std::is_same<GenMap, decay_t<T>>::value>>
  GenMapValue(std::shared_ptr<T> t_ptr);

  /** \brief Make an GenMapValue from an RCPWrapper */
  template <typename T,
            typename = krims::enable_if_t<!std::is_same<GenMap, decay_t<T>>::value>>
  GenMapValue(RCPWrapper<T> t_ptr);

  /** Make an GenMapValue from a Subscribable object (which is not a GenMap) */
  template <typename T,
            typename std::enable_if<std::is_base_of<Subscribable, T>::value &&
                                          !IsCheaplyCopyable<T>::value &&
                                          !std::is_same<GenMap, decay_t<T>>::value,
                                    int>::type = 0>
  GenMapValue(T& t);

  /** Make an GenMapValue from an rvalue reference */
  template <typename T,
            typename = typename std::enable_if<
                  !std::is_reference<T>::value && !IsCheaplyCopyable<T>::value &&
                  !std::is_same<GenMap, decay_t<T>>::value>::type>
  GenMapValue(T&& t) : GenMapValue{std::make_shared<T>(std::move(t))} {}
  // Note about the enable_if:
  //   - We need to make sure that T is the actual type (and not a
  //     reference)
  //   - T should not be cheap to copy (else first constructor applies)
  //   - T should not be a GenMap (we do not want maps in maps)

  GenMapValue(const GenMapValue&) = default;
  GenMapValue(GenMapValue&&)      = default;
  ~GenMapValue()                  = default;
  GenMapValue& operator=(const GenMapValue&) = default;
  GenMapValue& operator=(GenMapValue&&) = default;

  /** Obtain a non-const pointer to the internal object */
  template <typename T>
  RCPWrapper<T> get_ptr();

  /** Obtain a const pointer to the internal object */
  template <typename T>
  RCPWrapper<const T> get_ptr() const;

  /** Obtain a reference to the internal object */
  template <typename T>
  T& get() {
    return *get_ptr<T>();
  }

  /** Obtain a const reference to the internal object */
  template <typename T>
  const T& get() const {
    return *get_ptr<T>();
  }

  /** Is the object empty */
  bool empty() const { return m_object_ptr_ptr == nullptr; }

  /** Return the demangled typename of the type of the internal object.
   *
   *  \note This function only returns a sensible value if the
   *  code is compiled in DEBUG mode and if type demangling
   *  is supported by the OS (i.e. if KRIMS_HAVE_LIBSTDCXX_DEMANGLER
   *  is set).
   *
   *  Otherwise either type demangling is not possible or
   *  for performance reasons no type information is stored
   *  in this object.
   *
   *  In RELEASE builds this function always returns
   *  the string "<no_typeinfo_available>".
   */
  std::string type_name() const {
#ifdef DEBUG
    return demangled_string(m_type_name);
#else
    return "<no_typeinfo_available>";
#endif
  }

 private:
  //! Stupidly copy the object and set the m_object_ptr_ptr
  template <typename T>
  void copy_in(T t);

#ifdef DEBUG
  /** Check whether the object pointer stored in m_object_ptr_ptr
   *  can be obtained as a RCPWrapper<T>
   */
  template <typename T>
  constexpr bool can_get_value_as() const {
    typedef typename std::remove_const<T>::type nonconstT;

    // Allow if the type is identical to the type originally stored
    // or if a simple addition of const does the trick.
    return m_type_name == std::string(typeid(T).name()) ||
           m_type_name == std::string(typeid(nonconstT).name());
  }
#endif  // DEBUG

  /** The stored pointer to the RCPWrapper<T>
   * In other words: Twice dereferencing this will always
   * get us the object back.
   */
  std::shared_ptr<void> m_object_ptr_ptr;

#ifdef DEBUG
  std::string m_type_name;
#endif  // DEBUG
};

//
// ----------------------------------------------------------------
//

template <typename T, typename>
GenMapValue::GenMapValue(std::shared_ptr<T> t_ptr) {
  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));
#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T, typename>
GenMapValue::GenMapValue(RCPWrapper<T> t_ptr) {
  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));
#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T,
          typename std::enable_if<std::is_base_of<Subscribable, T>::value &&
                                        !IsCheaplyCopyable<T>::value &&
                                        !std::is_same<GenMap, decay_t<T>>::value,
                                  int>::type>
GenMapValue::GenMapValue(T& t) {
  SubscriptionPointer<T> t_ptr = make_subscription(t, "GenMapValue");

  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));

#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
void GenMapValue::copy_in(T t) {
  // Make a shared pointer out of T:
  auto t_ptr = std::make_shared<T>(std::move(t));

  // Now enwrap the t_ptr inside a RCPWrapper and make
  // a shared_ptr to the Wrapper, then set it
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(t_ptr);

#ifdef DEBUG
  // Keep an eye on the type name
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
RCPWrapper<T> GenMapValue::get_ptr() {
  assert_dbg(!empty(), ExcInvalidPointer());
  assert_dbg(can_get_value_as<T>(),
             ExcWrongTypeRequested(real_typename<T>(), type_name()));

  // We need to cast and then dereference to get the RCPWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<RCPWrapper<T>>(m_object_ptr_ptr);
}

template <typename T>
RCPWrapper<const T> GenMapValue::get_ptr() const {
  assert_dbg(!empty(), ExcInvalidPointer());
  assert_dbg(can_get_value_as<const T>(),
             ExcWrongTypeRequested(real_typename<T>(), type_name()));

  // We need to cast and then dereference to get the RCPWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<RCPWrapper<const T>>(m_object_ptr_ptr);
}

}  // namespace detail
}  // namespace krims
