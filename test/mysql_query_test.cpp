#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>
 
#include "connection.hpp"
#include "conns_manage.hpp"

BOOST_AUTO_TEST_SUITE(mysql_query_test)

BOOST_AUTO_TEST_CASE(mysql_query_test)
{
    aisqlpp::conns_manage mng(3);
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);
    string str = "SELECT count(uuid) FROM v5_law_text";
    cout << ptr->execute_query_count(str) << endl;

    assert(ptr->execute_check_exist(str));
    
    cout << "simple_request_free FINISHED!" << endl;
}


BOOST_AUTO_TEST_SUITE_END()
