#include "demangle.hh"
#include <cstring>
#include <stdlib.h>
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
#include <cxxabi.h>
#endif

namespace krims {

char* demangle(const char* mangled_name) {
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
  // try to demangle the function name:
  int status;
  char* p = abi::__cxa_demangle(mangled_name, 0, 0, &status);

  if (status == 0) {
    return p;
  } else {
    return NULL;
  }
#else
  char* ret = static_cast<char*>(malloc(strlen(mangled_name)));
  if (ret == NULL) return NULL;
  strcpy(ret, mangled_name);
  return ret;
#endif
}

std::string demangled_string(const char* mangled) {
  char* demangled = demangle(mangled);

  if (demangled != NULL) {
    std::string ret(demangled);
    free(demangled);
    return ret;
  } else {
    return std::string(mangled);
  }
}

}  // end namespace
