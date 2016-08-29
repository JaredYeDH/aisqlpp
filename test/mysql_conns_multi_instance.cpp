#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>

#include <vector>
 
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

#include <boost/ref.hpp>
#include <boost/bind.hpp>

using namespace aisqlpp::db_config;
using aisqlpp::connection_ptr;

BOOST_AUTO_TEST_SUITE(mysql_conns_multi_instances)

BOOST_AUTO_TEST_CASE(mysql_conns_multi_simple)
{
    aisqlpp::conns_manage mng1(3, host, user, passwd, db); 
    aisqlpp::conns_manage mng2(7, host, user, passwd, db); 

    assert(mng1.free_count() == 3);
    assert(mng2.free_count() == 7);

    {
        connection_ptr p1 = mng1.request_conn();
        connection_ptr p2 = mng1.request_conn();
        connection_ptr p3 = mng1.request_conn();
        connection_ptr p4 = mng1.try_request_conn(100);

        connection_ptr p5 = mng2.try_request_conn(100);
        connection_ptr p6 = mng2.request_conn();
        assert(p4 == nullptr);
        mng1.free_conn(p2);
        assert(mng1.free_count() == 1);
        assert(mng2.free_count() == 5);

        connection_ptr p7 = mng2.request_conn();
    }
    
    assert(mng1.free_count() == 1);
    assert(mng2.free_count() == 4);
        
    cout << "mysql_conns_multi_simple FINISHED!" << endl;
}

BOOST_AUTO_TEST_CASE(mysql_conns_multi_stage2)
{
    aisqlpp::conns_manage mng1(3, host, user, passwd, db); 
    aisqlpp::conns_manage mng2(7, host, user, passwd, db); 
    aisqlpp::conns_manage mng3(3, host, user, passwd, db); 

    assert(mng1.free_count() == 3);

    {
        assert(mng1.free_count() == 3);
        connection_ptr ptr;
        bool ret = mng1.request_scoped_conn(ptr);
        assert(ret && ptr);

        // reset should be free first
        ptr.reset();
        assert(!ptr);

        connection_ptr ptr2;
        ret = mng1.request_scoped_conn(ptr2);

        assert(mng1.free_count() == 2);

        ret = mng2.request_scoped_conn(ptr);

        assert(mng1.free_count() == 2);
        assert(mng2.free_count() == 6);

        mng1.conn_details();
        mng2.conn_details();

    }

    assert(mng1.free_count() == 3);
    assert(mng2.free_count() == 7);

    connection_ptr ptr;
    bool rett;
    rett = mng3.request_scoped_conn(ptr);
    assert(mng3.free_count() == 2);
    ptr.reset();
    assert(mng3.free_count() == 3);
    
    cout << "mysql_conns_multi_stage2 FINISHED!" << endl;
}


BOOST_AUTO_TEST_CASE(mysql_conns_multi_stage3)
{
    aisqlpp::conns_manage mng1(4, host, user, passwd, db); 

    assert(mng1.free_count() == 4);

    {
        assert(mng1.free_count() == 4);
        connection_ptr ptr;
        bool ret = mng1.request_scoped_conn(ptr);
        assert(ret && ptr);

        // reset should be free first
        //ptr.reset();
        //assert(!ptr);

        ret = mng1.request_scoped_conn(ptr);
        
        cout << "SCOPED END FREE:" << mng1.free_count() << endl;
        assert(mng1.free_count() == 3);

    }

    cout << "END FREE:" << mng1.free_count() << endl;
    assert(mng1.free_count() == 4);

    cout << "mysql_conns_multi_stage3 FINISHED!" << endl;
}

BOOST_AUTO_TEST_SUITE_END()
