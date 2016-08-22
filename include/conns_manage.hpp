#ifndef _CONNS_MANAGE_HPP_
#define _CONNS_MANAGE_HPP_

#include <boost/noncopyable.hpp>
#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "connection.hpp"

namespace aisqlpp {

namespace db_config {
    static const string host = "192.168.1.233";
    static const string user = "v5kf";
    static const string passwd = "v5kf";
    static const string db = "v5_law";
}

enum conn_stat {
    conn_working = 1,
    conn_pending = 2,
    conn_error   = 3,
};

class conns_manage: public boost::noncopyable
{
public:
    conns_manage() = delete;

    explicit conns_manage(size_t capacity);
    connection_ptr request_conn();
    void free_conn(connection_ptr conn);

    static boost::condition_variable_any conn_notify;
    static boost::mutex conn_notify_mutex;
    
private:
    size_t capacity_;
    std::map<connection_ptr, enum conn_stat> conns_;
};


}

#endif  // _CONNS_MANAGE_HPP_
