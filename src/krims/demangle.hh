#pragma once
#include <string>
#include <typeinfo>

namespace krims {
/** \brief Demangle a c++ symbol name
 *
 * \param mangled_name    The mangled name of the symbol
 * \returns  The demangled name of the symbol. The char is allocated by
 *           malloc and should be freed after use.
 *
 *           If something goes wrong the function returns NULL.
 *
 * \note This function is not memory-safe as the returned pointer needs
 * to be freed by the user.
 * */
char* demangle(const char* mangled_name);

/** Return the demangled name of the symbol represented by the argument
 * as a std::string (This function is memory-safe)*/
std::string demangled_string(const char* mangled);

/** Return the demangled name of the symbol represented by the argument
 * as a std::string (This function is memory-safe)*/
inline std::string demangled_string(const std::string& mangled_name) {
  return demangled_string(mangled_name.c_str());
}

/** Return the demangled typename of the type T (This function is memory-safe)*/
template <typename T>
std::string real_typename() {
  return demangled_string(typeid(T).name());
}

}  // namespace krims
