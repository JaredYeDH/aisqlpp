#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>
 

#include "connection.hpp"
#include "conns_manage.hpp"

BOOST_AUTO_TEST_SUITE(request_free_test)

BOOST_AUTO_TEST_CASE(simple_request_free)
{
    aisqlpp::conns_manage mng(5);

    BOOST_CHECK_EQUAL(5, mng.free_count());
    
    aisqlpp::connection_ptr ptr1 = mng.request_conn();
    aisqlpp::connection_ptr ptr2 = mng.request_conn();

    BOOST_CHECK_EQUAL(3, mng.free_count());

    mng.free_conn(ptr1);
    BOOST_CHECK_EQUAL(4, mng.free_count());
    mng.free_conn(ptr2);
    BOOST_CHECK_EQUAL(5, mng.free_count());

    ptr2 = mng.try_request_conn(1000);
    assert(ptr2 != nullptr);
    BOOST_CHECK_EQUAL(4, mng.free_count());
    
    mng.conn_check();
    mng.free_conn(ptr2);

}


BOOST_AUTO_TEST_SUITE_END()
