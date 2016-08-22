#include "general.hpp"
#include "connection.hpp"

namespace aisqlpp {

    connection::connection(string host, string user, string passwd, string db):
        driver_(get_driver_instance())
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
        BOOST_LOG_T(info) << "Create New Connection OK!" ;
    }

    connection::~connection()
    {
        /* reset to fore delete, actually not need */
        conn_.reset();
        stmt_.reset();
    }
}
