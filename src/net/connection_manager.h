/*
 * connection_manager.h
 *
 */

#ifndef SRC_NET_CONNECTION_MANAGER_H_
#define SRC_NET_CONNECTION_MANAGER_H_

#include <map>
#include <list>
#include "net/connection.h"

namespace kit {

class ConnectionManager {
 public:
  ConnectionManager();
  ~ConnectionManager();

  void Add(const ConnectionPtr& conn);
  void Remove(const ConnectionPtr& conn);
  void StopAll();

 private:
  typedef std::map<std::string, ConnectionPtr> ConnectionMap;
  ConnectionMap connections_;

  ConnectionManager(const ConnectionManager&);
  void operator=(const ConnectionManager&);
};

}

#endif /* SRC_NET_CONNECTION_MANAGER_H_ */
