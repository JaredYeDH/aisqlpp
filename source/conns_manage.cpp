#include "general.hpp"
#include "conns_manage.hpp"

#include <boost/format.hpp>

namespace aisqlpp {

 boost::condition_variable_any conns_manage::conn_notify;
 boost::mutex conns_manage::conn_notify_mutex;

conns_manage::conns_manage(size_t capacity):
    capacity_(capacity),
    conns_()
{
    assert(capacity_);

    for (int i=0; i < capacity_; ++i)
    {
        connection_ptr new_c = std::make_shared<connection>(db_config::host, db_config::user,
                                                            db_config::passwd, db_config::db); 
        conns_[new_c] = conn_error;  // not working
    }

    free_cnt_ = 0;
    for (auto& item: conns_)
    {
        ++ free_cnt_;
        conns_[item.first] = conn_pending;
    }

    BOOST_LOG_T(info) << "TOTAL FREE CONN AVAIL:" << free_cnt_;
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
            return item.first;
        }
    }

    // should not be here!!!!
    abort();
    return nullptr;
}

void conns_manage::free_conn(connection_ptr conn)
{
    {
        boost::lock_guard<boost::mutex> lock(conn_notify_mutex);

        for (auto& item: conns_)
        {
            if (item.first == conn)
            {
                assert(item.second != conn_pending);
                item.second = conn_pending;
                ++ free_cnt_;
            }
        }

        ++ free_cnt_;
    }
    conn_notify.notify_all();

    return;
}

// dummy connection health check
void conns_manage::conn_check()
{
    size_t cnt_total;
    size_t cnt_pending;
    size_t cnt_working;
    size_t cnt_error;

    for (auto& item: conns_)
    {
        if (conns_[item.first] == conn_pending)
            ++ cnt_pending;
        else if (conns_[item.first] == conn_working)
            ++ cnt_working;
        else if (conns_[item.first] == conn_error) 
            ++ cnt_error;

        ++ cnt_total;
    }

    assert(cnt_total == capacity_);
    if (free_cnt_ != cnt_pending)
        BOOST_LOG_T (error) << boost::format("record free conn:%d, but check with: %d ") % free_cnt_ 
                                            % cnt_pending;

    BOOST_LOG_T(info) << boost::format("Total: %d, avail: %d, working: %d , error: %d") % cnt_total 
                                            % cnt_pending % cnt_working % cnt_error;

    return;
}

}
