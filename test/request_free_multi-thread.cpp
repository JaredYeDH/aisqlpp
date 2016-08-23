#include <boost/test/unit_test.hpp>
#include <string>
#include <memory>
 
#include <boost/thread.hpp>

#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

using namespace aisqlpp::db_config;

BOOST_AUTO_TEST_SUITE(request_free_multithread_test)

static void dummy_t1(aisqlpp::conns_manage& mng)
{
    std::vector<aisqlpp::connection_ptr> vec;

    while (mng.free_count() > 0) 
        vec.push_back(mng.request_conn()); 

    assert(mng.free_count() == 0);
    cout << "slow free connection!" << endl;

    for (auto& item: vec)
    {
        sleep(2);
        cout << "FREE:" << item->get_uuid() << endl;
        mng.free_conn(item);
    }

}

static void dummy_t2(aisqlpp::conns_manage& mng)
{
    size_t free_desire = 0;
    aisqlpp::connection_ptr ptr = nullptr;

    while (free_desire < mng.capacity())
    {
        ptr = mng.request_conn();
        cout << "GET:" << ptr->get_uuid() << endl;
        ++ free_desire;
    }
}

BOOST_AUTO_TEST_CASE(simple_request_free_multithread)
{
    boost::thread_group threads;
    aisqlpp::conns_manage mng(5, host, user, passwd, db); 

    threads.create_thread(
            [&mng]{
            cerr<<"THREAD_ID:" <<boost::this_thread::get_id()<<endl;
            dummy_t1(mng);
        });

    sleep(2);

    threads.create_thread(
            [&mng]{
            cerr<<"THREAD_ID:" <<boost::this_thread::get_id()<<endl;
            dummy_t2(mng);
        });

    threads.join_all();

    cout << "simple_request_free_multithread FINISHED! " << endl;

    return;
}


BOOST_AUTO_TEST_SUITE_END()
