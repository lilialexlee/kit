/*
 * process.h
 *
 */

#ifndef SRC_UTIL_PROCESS_H_
#define SRC_UTIL_PROCESS_H_

namespace kit {

void Daemonize(void);

void CreatePidFile(const char* pid_file);

bool IsProcessExist(pid_t pid);

}

#endif /* SRC_UTIL_PROCESS_H_ */
