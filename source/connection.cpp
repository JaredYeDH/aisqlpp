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
            cerr << "# ERR: " << e.what();
            cerr << " (MySQL error code: " << e.getErrorCode();
		    cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
            abort();
        }

        stmt_->execute("USE " + db);
        cout << "Create New Connection OK!" << endl;
    }

    connection::~connection()
    {
        /* reset to fore delete, actually not need */
        conn_.reset();
        stmt_.reset();
    }
}
