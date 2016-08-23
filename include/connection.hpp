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

    /**
     * SQL API
     */
    bool execute_command(const string& sql);
    bool execute_query(const string& sql);
    size_t execute_query_count(const string& sql);
    bool execute_check_exist(const string& sql);

private:
    sql::Driver* driver_;

    size_t  conn_uuid_; // RAND_MAX
    boost::shared_ptr<sql::Connection>  conn_;
    boost::shared_ptr<sql::Statement>   stmt_;

    // 因为都是获取连接之后查询，然后释放连接，所以这里
    // 是线程安全的，在下一次查询需要使用result_set的时候
    // 先进行reset()就可以清空之前的查询数据集，接收新的结果
    // 所以这里不会导致内存泄漏
    boost::shared_ptr<sql::ResultSet>   result_;

    // prep_stmt_ create manually
    // boost::shared_ptr< sql::PreparedStatement > prep_stmt_;

    // may be used future
    conns_manage&    manage_;
};

}

#endif  // _CONNECTION_HPP_
