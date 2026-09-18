#include "connection.h"
#include <map>
#include "Singleton.h"
class ConnectionMgr:public Singleton<ConnectionMgr>
{
public:
    friend Singleton<ConnectionMgr>;
    ConnectionMgr(const ConnectionMgr&) = delete;
    ConnectionMgr& operator=(const ConnectionMgr&) = delete;

    
    void AddConnection(std::shared_ptr<Connection> conn);
    void RemoveConnection(const std::string &conn_id_str);
private:
    ConnectionMgr();
    std::map<std::string,std::shared_ptr<Connection>> _connections;
};