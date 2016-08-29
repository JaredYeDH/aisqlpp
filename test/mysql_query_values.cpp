#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>

#include <vector>

#include <boost/format.hpp>
 
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

using namespace aisqlpp::db_config;

BOOST_AUTO_TEST_SUITE(mysql_query_test_values)

BOOST_AUTO_TEST_CASE(mysql_template_querys)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();
    assert(ptr);
  
    string str = "SELECT uuid, YGSC, FYPJ, SSZY FROM v5_law_text WHERE uuid=3;";  
    uint64_t uuid_r;
    string   ygsc_r;
    string   fypj_r;
    string   sszy_r;

    assert(ptr->execute_query_values(str, uuid_r, ygsc_r, fypj_r, sszy_r)); 
    cout << "查询信息：" << endl;
    cout << boost::format("查询信息: %llu, %s, %s, %s") %uuid_r%ygsc_r%sszy_r%sszy_r << endl;
    cout << "Finishied!" << endl;
}



BOOST_AUTO_TEST_SUITE_END()
