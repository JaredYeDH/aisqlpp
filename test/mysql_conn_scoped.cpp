#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>

#include <vector>
 
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

#include <boost/ref.hpp>
#include <boost/bind.hpp>

using namespace aisqlpp::db_config;

BOOST_AUTO_TEST_SUITE(mysql_conn_scoped)

BOOST_AUTO_TEST_CASE(mysql_conn_scoped_normal_test)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 
    aisqlpp::connection_ptr ptr = mng.request_conn();

    {
        boost::shared_ptr<aisqlpp::connection> wrap_ptr(ptr.get(), 
                                                        boost::bind(&aisqlpp::conns_manage::free_conn, 
                                                        boost::ref(mng), ptr));
        assert(mng.free_count() == 2);
    }

    assert(mng.free_count() == 3);
    
    cout << "mysql_conn_scoped_normal_test FINISHED!" << endl;
}

BOOST_AUTO_TEST_CASE(mysql_conn_scoped_test)
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 

    {
        aisqlpp::connection_ptr ptr;
        bool ret = mng.request_scoped_conn(ptr);
        assert(ptr);
        cout << "Reference count:" << ptr.use_count() << endl;

        mng.conn_details();
    }

    mng.conn_details();

    assert(mng.free_count() == 3);
    
    cout << "mysql_conn_scoped_test FINISHED!" << endl;
}




BOOST_AUTO_TEST_SUITE_END()
