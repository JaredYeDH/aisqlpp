#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>

#include <vector>
 
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

using namespace aisqlpp::db_config;

BOOST_AUTO_TEST_SUITE(mysql_query_test)

BOOST_AUTO_TEST_CASE(mysql_query_test)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);
    string str = "SELECT count(uuid) FROM v5_law_text";
    cout << ptr->execute_query_count(str) << endl;

    assert(ptr->execute_check_exist(str));
    
    cout << "simple_request_free FINISHED!" << endl;
}


BOOST_AUTO_TEST_CASE(mysql_query_resultset_test)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);
    string str = "SELECT YGSC, SSZY FROM v5_law_text WHERE uuid=1 or uuid=2;";
    if (ptr->execute_query(str))
    {
        sql::ResultSet *res = ptr->get_result_set();
        while (res->next()) 
        {
            cout << res->getString(1) << endl;
            cout << res->getString(2) << endl;
        }
    }

    return;
}

BOOST_AUTO_TEST_CASE(mysql_template_one_query)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);

  
    string str = "SELECT YGSC FROM v5_law_text WHERE uuid=2;";  
    string ret_str;
    
    assert(ptr->execute_query_value(str, ret_str));
    cout << ret_str << endl;

    int ret_int;
    str = "SELECT uuid FROM v5_law_text WHERE uuid=5+4;";
    assert(ptr->execute_query_value(str, ret_int));
    assert(ret_int == 9);
    cout << ret_int << endl;
}

BOOST_AUTO_TEST_CASE(mysql_template_column_query)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    assert(ptr);
    string str = "SELECT YGSC FROM v5_law_text WHERE uuid>2 and uuid<7;";
    std::vector<string> vec_str;
    
    assert(ptr->execute_query_column(str, vec_str));
    assert(vec_str.size() == 4);
    for (auto & item: vec_str)
        cout << item << endl;
 
    std::vector<int> vec_int;
    str = "SELECT uuid FROM v5_law_text WHERE uuid>2 and uuid<18;";
    assert(ptr->execute_query_column(str, vec_int));
    assert(vec_int.size() == 15);
    for (auto & item: vec_int)
        cout << item << endl;

    return;
}


BOOST_AUTO_TEST_SUITE_END()
