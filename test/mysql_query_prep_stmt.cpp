#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>

#include <vector>
 
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

using namespace aisqlpp::db_config;

BOOST_AUTO_TEST_SUITE(mysql_query_test_prep_stmt)


BOOST_AUTO_TEST_CASE(mysql_query_resultset_test)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);
    string str = "SELECT uuid, YGSC, SSZY FROM v5_law_text WHERE uuid=? or uuid=?;";
    ptr->create_prep_stmt(str);
    ptr->get_prep_stmt()->setInt(1, 2);
    ptr->get_prep_stmt()->setInt(2, 29);
    if (ptr->execute_prep_stmt_query())
    {
        sql::ResultSet *res = ptr->get_result_set();
        while (res->next()) 
        {
            cout << res->getInt(1) << endl;
            cout << res->getString(2) << endl;
            cout << res->getString(3) << endl;
        }
    }

    return;
}


BOOST_AUTO_TEST_SUITE_END()
