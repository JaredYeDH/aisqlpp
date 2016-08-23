#include "general.hpp"
#include "connection.hpp"
#include "conns_manage.hpp"

#include <boost/scoped_ptr.hpp>

namespace aisqlpp {

connection::connection(conns_manage& manage, size_t conn_uuid,
                       string host, string user, string passwd, string db):
    driver_(get_driver_instance()),
    conn_uuid_(conn_uuid),
    result_(),
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


// 字符串特例化
bool connection::execute_query_column(const string& sql, std::vector<std::string>& vec)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        if (result_->rowsCount() == 0)
            return false;

        vec.clear();
        while (result_->next()) 
        {
            vec.push_back(static_cast<std::string>(result_->getString(1)));

        }

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


bool connection::execute_query_value(const string& sql, std::string& val)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        if (result_->rowsCount() == 0)
            return false;

        if (result_->rowsCount() != 1) 
        {
            BOOST_LOG_T(error) << "Error rows count:" << result_->rowsCount() << endl;
            return false;
        }

        while (result_->next()) 
        {
            val = static_cast<std::string>(result_->getString(1));
        }

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

}
