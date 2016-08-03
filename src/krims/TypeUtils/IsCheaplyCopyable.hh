#pragma once
#include "IsComplexNumber.hh"
#include <string>

namespace krims {
//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether it is cheap to copy the type T
 *
 *  All arithmetic types (float, double, long double, all integers), std::string
 *  and std::complex types are currently accepted as cheaply copyable.
 **/
template <typename T>
struct IsCheaplyCopyable
      : public std::integral_constant<
              bool,
              std::is_arithmetic<T>::value ||
                    std::is_same<std::string,
                                 typename std::remove_cv<T>::type>::value ||
                    IsComplexNumber<T>::value> {};
//@}

}  // namespace krims
