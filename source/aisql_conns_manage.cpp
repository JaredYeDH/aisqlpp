#include "aisql_general.hpp"
#include "aisql_conns_manage.hpp"

#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>

#include <ctime>

namespace aisqlpp {

size_t conns_manage::generate_conn_uuid(const conns_manage& mng)
{
    if (mng.conns_.size() >= RAND_MAX)
    {
        cerr << "TOO MUCH CONNECTIONS" << endl;
        abort();
    }

    std::srand(std::time(0));
    size_t r_id = 0;

    while (true)
    {
        r_id = std::rand();
        bool retry_flag = false;
        for (const auto& item: mng.conns_)
        {
            if (item.first->get_uuid() == r_id)
            {
                retry_flag = true;
                break;
            }
        }
        if (retry_flag)
            continue;

        return r_id;
    }
}

conns_manage::conns_manage(size_t capacity, const string host, const string user,
                          const string passwd, const string db):
    capacity_(capacity),
    conns_()
{
    assert(capacity_);

    for (int i=0; i < capacity_; ++i)
    {
        connection_ptr new_c = 
            boost::make_shared<connection>(*this, generate_conn_uuid(*this),
                                           host, user, passwd, db);
        conns_[new_c] = conn_error;  // not working
    }

    free_cnt_ = 0;
    for (auto& item: conns_)
    {
        ++ free_cnt_;
        conns_[item.first] = conn_pending;
    }

    BOOST_LOG_T(info) << "TOTAL FREE CONN AVAIL:" << free_cnt_ << endl;
    return;
}

connection_ptr conns_manage::request_conn()
{
    boost::unique_lock<boost::mutex> lock(conn_notify_mutex); 
    //owns lock

    while(free_cnt_ == 0)
        conn_notify.wait(lock);

    for (auto& item: conns_)
    {
        if (item.second == conn_pending)
        {
            item.second = conn_working;
            ++ aquired_time_;
            -- free_cnt_;

            BOOST_LOG_T(debug) << "Request connection:" << item.first->get_uuid();
            return item.first;
        }
    }

    // should not be here!!!!
    abort();
    return nullptr;
}


bool conns_manage::request_scoped_conn(connection_ptr& conn)
{
    boost::unique_lock<boost::mutex> lock(conn_notify_mutex); 
    //owns lock

    while(free_cnt_ == 0)
        conn_notify.wait(lock);

    for (auto& item: conns_)
    {
        if (item.second == conn_pending)
        {
            item.second = conn_working;
            ++ aquired_time_;
            -- free_cnt_;

            conn.reset( item.first.get(), 
                        boost::bind(&conns_manage::free_conn, 
                        this, boost::ref(conn))); //返回之后已经初始化了

            BOOST_LOG_T(debug) << "Request guard connection:" << conn->get_uuid();
            return true;
        }
    }

    // should not be here!!!!
    abort();
    return false;
}

connection_ptr conns_manage::try_request_conn(size_t msec)
{
    boost::unique_lock<boost::mutex> lock(conn_notify_mutex); 
    //owns lock

    if (!free_cnt_ && !msec)
        return nullptr;

    // timed_wait not work with 0
    if(free_cnt_ || conn_notify.timed_wait(lock, boost::posix_time::milliseconds(msec)))
    {
        for (auto& item: conns_)
        {
            if (item.second == conn_pending)
            {
                item.second = conn_working;
                ++ aquired_time_;
                -- free_cnt_;

                BOOST_LOG_T(debug) << "Request connection:" << item.first->get_uuid();
                return item.first;
            }
        }
    }

    return nullptr;
}



// f (a < b) is false and (b < a) is false, then (a == b). 
// This is how STL's find() works.
void conns_manage::free_conn(connection_ptr& conn)
{ 
    {
        boost::lock_guard<boost::mutex> lock(conn_notify_mutex);

        auto it = conns_.find(conn);    //operator < 值比较
        if (it != conns_.end())
        {
            assert(it->second = conn_pending);
            it->second = conn_pending;
            ++ free_cnt_;

            BOOST_LOG_T(debug) << "Free connection:" << it->first->get_uuid();
        }
        else
        {
            BOOST_LOG_T(debug) << "Free not found!!!:" << it->first->get_uuid();
        }
    }
    conn_notify.notify_all();

    return;
}

enum conn_stat conns_manage::get_conn_stat(const connection_ptr& conn)
{
    const auto it = conns_.find(conn);
    if (it != conns_.end())
        return it->second;

    return conn_error;
}

// dummy connection health check
void conns_manage::conn_check()
{
    size_t cnt_total = 0;
    size_t cnt_pending = 0;
    size_t cnt_working = 0;
    size_t cnt_error = 0;

    for (auto& item: conns_)
    {
        if (conns_[item.first] == conn_pending)
            ++ cnt_pending;
        else if (conns_[item.first] == conn_working)
            ++ cnt_working;
        else if (conns_[item.first] == conn_error) 
            ++ cnt_error;
        else
            abort();

        ++ cnt_total;
    }

    assert(cnt_total == capacity_);

    if (free_cnt_ != cnt_pending)
        BOOST_LOG_T (error) << boost::format("record free conn:%d, but check with: %d ") % free_cnt_ 
                                            % cnt_pending;

    BOOST_LOG_T(info) << boost::format("Total: %d, avail: %d, working: %d , error: %d") % cnt_total 
                                            % cnt_pending % cnt_working % cnt_error << endl;

    return;
}


void conns_manage::conn_details()
{
    for (const auto& item: conns_)
    {
        cout << "ID: " << item.first->get_uuid() << ", refer: " << 
            item.first.use_count() << ", stat: " << get_conn_stat(item.first) << endl;
    }

    return;
}

}
