#include "aisql_general.hpp"
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

#include <boost/scoped_ptr.hpp>

namespace aisqlpp {

connection::connection(conns_manage& manage, size_t conn_uuid,
                       string host, string user, string passwd, string db):
    driver_(get_driver_instance()),
    conn_uuid_(conn_uuid),
    result_(),
    prep_stmt_(),
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

    stmt_->execute("USE " + db + ";");
    BOOST_LOG_T(info) << "Create New Connection " << conn_uuid_ << " OK!" ;
}

connection::~connection()
{
    /* reset to fore delete, actually not need */
    conn_.reset();
    stmt_.reset();
    prep_stmt_.reset();

    if (result_)
    {
        assert(result_.unique());
        result_.reset();
    }
    //BOOST_LOG_T(info) << "Destruct Connection " << conn_uuid_ << " OK!" ;
}


bool connection::execute_command(const string& sql)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        return stmt_->execute(sql);

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " STMT: " << sql << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}

bool connection::execute_query(const string& sql)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        
        // 没有结果以及出错的时候，返回false
        if (result_->rowsCount() == 0)
            return false;

        return true;

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " STMT: " << sql << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}

// return 0 maybe error, we ignore this case
size_t connection::execute_query_count(const string& sql)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        if (result_->rowsCount() != 1)
            return 0;

        result_->next();
        return (result_->getInt(1));

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " STMT: " << sql << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return 0;
    }
}

bool connection::execute_check_exist(const string& sql)
{
    if (execute_query_count(sql) >=0 )
        return true;

    return false;
}



bool connection::execute_prep_stmt_command()
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        return prep_stmt_->execute(); 

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " PREP_STMT: " << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}

bool connection::execute_prep_stmt_query()
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        prep_stmt_->execute();
        result_.reset(prep_stmt_->getResultSet());
        
        // 没有结果以及出错的时候，返回false
        if (result_->rowsCount() == 0)
            return false;

        return true;

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " PREP_STMT: "  << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}


}
