#include "ConnectionMgr.h"

ConnectionMgr::ConnectionMgr()
{
}

void ConnectionMgr::AddConnection(std::shared_ptr<Connection> conn)
{
    _connections[conn->GetUUID()] = conn;
}
void ConnectionMgr::RemoveConnection(const std::string &conn_id_str)
{
    _connections.erase(conn_id_str);
}