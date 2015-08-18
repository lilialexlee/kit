/*
 * connection_manager.cc
 *
 */

#include "net/connection_manager.h"

namespace kit {

ConnectionManager::ConnectionManager() {

}

ConnectionManager::~ConnectionManager() {

}

void ConnectionManager::Add(const ConnectionPtr& conn) {
  connections_[conn->GetUniqueIdentifier()] = conn;
}

void ConnectionManager::Remove(const ConnectionPtr& conn) {
  connections_.erase(conn->GetUniqueIdentifier());
}

void ConnectionManager::StopAll() {
  for (ConnectionMap::iterator it = connections_.begin();
      it != connections_.end(); ++it) {
    it->second->Stop();
  }
}

}
