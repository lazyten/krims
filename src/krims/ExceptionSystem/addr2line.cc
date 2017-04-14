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

#include "addr2line.hh"
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace krims {

#ifdef KRIMS_ADDR2LINE_AVAILABLE
int addr2line(const char* execname, const char* addr, const size_t maxlen, char* codefile,
              char* number) {
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

    // Call addr2line (NOLINT: ignore the lines for clang-tidy)
    if (execlp("addr2line", "addr2line", addr, "-e", execname,  // NOLINT
               reinterpret_cast<void*>(NULL)) == -1) {          // NOLINT
      std::abort();
    }
  }

  // Parent process receives on pipe[0]
  close(pipefd[1]);

  // Read data from the pipe
  const ssize_t len = read(pipefd[0], codefile, maxlen);
  close(pipefd[0]);

  // Wait for addr2line to be done
  if (waitpid(pid, nullptr, 0) != pid) {
    // error wrong pid exited ... no way we can recover
    std::abort();
  }

  if (len <= 0) {
    // eof or error encountered when reading from pipe
    return -1;
  }

  // Make sure that we have a null-terminated string:
  // Note that the cast will always work, since len <= 0
  // has returned already.
  if (static_cast<size_t>(len) < maxlen) {
    codefile[len] = 0;
  } else {
    codefile[len - 1] = 0;
  }

  // Ignore tailing newline:
  char* newline = strchr(codefile, '\n');
  if (newline != nullptr) {
    *newline = 0;
  }

  // Find first colon and split there:
  char* colon = strchr(codefile, ':');
  if (colon == nullptr) {
    // ":" not found, use everything for codefile.
    return 0;
  } else {
    // colon points to the first occurrence of ":"
    strncpy(number, colon + 1, maxlen);

    // Assure that number ends on a 0:
    number[maxlen - 1] = 0;

    *colon = 0;

    if (strncmp("??", codefile, 2) == 0 && strncmp("0", number, 1) == 0) {
      // Error occurred finding the file and line,
      // since file is ?? and line is 0
      return -1;
    }

    return 0;
  }
}
#endif

}  // namespace krims
