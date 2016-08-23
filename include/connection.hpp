#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "general.hpp"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace aisqlpp {

class conns_manage;

class connection;
using connection_ptr = boost::shared_ptr<connection>;

class connection final: public boost::noncopyable
{
public:
    connection() = delete;
    connection(conns_manage& manage, size_t conn_uuid,
               string host, string user, string passwd, string db);
    ~connection();
    void set_uuid(size_t uuid) { conn_uuid_ = uuid; }
    size_t get_uuid() { return conn_uuid_; }

private:
    sql::Driver* driver_;

    size_t  conn_uuid_; // RAND_MAX
    boost::shared_ptr< sql::Connection > conn_;
    boost::shared_ptr< sql::Statement >  stmt_;

    // may be used future
    conns_manage&    manage_;
};

}

#endif  // _CONNECTION_HPP_
