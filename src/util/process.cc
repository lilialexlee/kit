/*
 * process.cc
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>

namespace kit {

void Daemonize(void) {
  if (fork() != 0)
    exit(0);  // parent exits
  setsid();  // create a new session
  // Every output goes to /dev/null.
  int fd = 0;
  if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > STDERR_FILENO)
      close(fd);
  }
}

void CreatePidFile(const char* pid_file) {
  FILE *fp = fopen(pid_file, "w");
  if (fp) {
    fprintf(fp, "%d\n", (int) getpid());
    fclose(fp);
  }
}

bool IsProcessExist(pid_t pid) {
  if (kill(pid, 0) == -1 && errno == ESRCH) {
    return true;
  }
  return false;
}

}
