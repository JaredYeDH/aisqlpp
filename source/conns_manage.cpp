#include "conns_manage.hpp"

namespace aisqlpp {

conns_manage::conns_manage(size_t capacity):
    capacity_(capacity),
    conns_()
{
    assert(capacity_);

    for (int i=0; i < capacity_; ++i)
    {
        connection_ptr new_c = std::make_shared<connection>(db_config::host, db_config::user,
                                                            db_config::passwd, db_config::db); 


    }

}


void free_conn(connection_ptr conn)
{

}


}
