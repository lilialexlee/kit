/*
 * file_util.cc
 *
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

namespace kit {

static int LockOrUnlock(int fd, bool lock) {
  errno = 0;
  struct flock f;
  memset(&f, 0, sizeof(f));
  f.l_type = (lock ? F_WRLCK : F_UNLCK);
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;  // Lock/unlock entire file
  return fcntl(fd, F_SETLK, &f);
}

int LockFile(int fd) {
  return LockOrUnlock(fd, true);
}

int UnlockFile(int fd) {
  return LockOrUnlock(fd, false);
}

}

