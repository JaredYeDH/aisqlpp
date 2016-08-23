#include "general.hpp"
#include "connection.hpp"
#include "conns_manage.hpp"

namespace aisqlpp {

connection::connection(conns_manage& manage, size_t conn_uuid,
                       string host, string user, string passwd, string db):
    driver_(get_driver_instance()),
    conn_uuid_(conn_uuid),
    manage_(manage)
{
    try {
        conn_.reset(driver_->connect(host, user, passwd));
        stmt_.reset(conn_->createStatement());
    } 
    catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << "# ERR: " << e.what();
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode();
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" ;
        abort();
    }

    stmt_->execute("USE " + db);
    BOOST_LOG_T(info) << "Create New Connection " << conn_uuid_ << " OK!" ;
}

connection::~connection()
{
    /* reset to fore delete, actually not need */
    conn_.reset();
    stmt_.reset();

    BOOST_LOG_T(info) << "Destruct Connection " << conn_uuid_ << " OK!" ;
}



}
