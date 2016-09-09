#pragma once

#if defined KRIMS_USE_ADDR2LINE && defined DEBUG && \
      defined KRIMS_HAVE_GLIBC_STACKTRACE
// It only makes sense to define addr2line if we actually can make use of its
// functionality, i.e. if we have a glibc stacktrace and are in DEBUG mode
// (Debug symbols) and the user actually wants to use the feature

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Now we defined addr2line and it is available.
#define KRIMS_ADDR2LINE_DEFINED

namespace krims {

/** Get the line of a file from the incoming address string
 *
 * \param execname   Path to the executable
 * \param addr       Address string
 * \param maxlen     The maximal length any of the strings may have
 * \param codefile   The file name string which contains the address
 * \param number     The line number in codefile which contains the address
 *
 * \returns -1 on any error, else 0
 *
 * This function is inspired by
 * https://github.com/vmarkovtsev/DeathHandler/blob/master/death_handler.cc
 */
static int addr2line(const char* execname, const char* addr,
                     const size_t maxlen, char* codefile, char* number) {
  // By default set number and codefile to empty string
  codefile[0] = 0;
  number[0] = 0;

  int pipefd[2];
  if (pipe(pipefd) != 0) return -1;

  pid_t pid = fork();
  if (pid == -1) {
    // Error forking
    return -1;
  } else if (pid == 0) {
    // The child feeds on pipe[1]
    // (from the process' stdout and stderr)
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);

    // Call addr2line
    if (execlp("addr2line", "addr2line", addr, "-e", execname,
               reinterpret_cast<void*>(NULL)) == -1) {
      abort();
    }
  }

  // Parent process receives on pipe[0]
  close(pipefd[1]);

  // Read data from the pipe
  ssize_t len = read(pipefd[0], codefile, maxlen);
  close(pipefd[0]);
  if (len == 0) {
    // eof encountered when reading from pipe
    return -1;
  }

  // Put a null at the end
  codefile[len] = 0;

  // Wait for addr2line to be done
  if (waitpid(pid, NULL, 0) != pid) {
    // error wrong pid exited ... no way we can recover
    abort();
  }

  // Split at ":"
  char* colon = strstr(codefile, ":");
  if (colon == NULL) {
    // ":" not found, use everything for codefile.
    return 0;
  } else {
    // colon points to the first occurrence of ":"
    strcpy(number, colon + 1);
    *colon = 0;
    return 0;
  }
}

}  // namespace krims
#endif
