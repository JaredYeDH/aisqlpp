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

namespace aisqlpp {

class connection;
using connection_ptr = std::shared_ptr<connection>;

class connection: public boost::noncopyable
{
public:
    connection(string host, string user, string passwd, string db);
    ~connection();

private:
    sql::Driver* driver_;
    std::shared_ptr< sql::Connection > conn_;
    std::shared_ptr< sql::Statement >  stmt_;

};

}

#endif  // _CONNECTION_HPP_
