stsqlpp

Aimed for wrapping a efficient, user-friendly and resource safe C++ MySQL/Mariadb client connector library.   
This library contains several functions:   
(1) Fixed size database connection pool, so user can request already-prepared database connection by blocking or non-blocking method;   
(2) All resources are encapsulated in the connection object, so can be reused the whole lifetime, and do not need to mind the probabily leakage problem;   
(3) With C++ template, some records can easily get just using a funtion call.
(4) Tried a Scoped-Conn, RAII theory helping you event not care about free connection in normal or exception routine.   

# Build requirement:   
C++11   
Boost   
mysql-connector-cpp   

# Quick Start:   
(1) Initialize specified size connection-pool.   
```cpp
#include "aisql_connection.hpp"
#include "aisql_conns_manage.hpp"

aisqlpp::conns_manage mng(3, host, user, passwd, db); 
```

(2) Require connection, then do your normal qurey   
```cpp
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

mng.free_conn(ptr); // free the connection
```

(3) prepared_stmt usage, interact with ResultSet directly. I will fix this latter, to make it easy use like above.   
```cpp
aisqlpp::connection_ptr ptr = mng.request_conn();

assert(ptr);
string str = "SELECT uuid, YGSC, SSZY FROM v5_law_text WHERE uuid=? or uuid=?;";
ptr->create_prep_stmt(str);
ptr->get_prep_stmt()->setInt(1, 2);
ptr->get_prep_stmt()->setInt(2, 29);
if (ptr->execute_prep_stmt_query())
{
    sql::ResultSet *res = ptr->get_result_set();
    while (res->next()) 
    {
        cout << res->getInt(1) << endl;
        cout << res->getString(2) << endl;
        cout << res->getString(3) << endl;
    }
}

mng.free_conn(ptr);  // free the connection
```

(4) RAII non-free method   
```cpp
aisqlpp::conns_manage mng1(4, host, user, passwd, db); 

assert(mng1.free_count() == 4);

{
    assert(mng1.free_count() == 4);
    connection_ptr ptr;
    bool ret = mng1.request_scoped_conn(ptr);
    assert(ret && ptr);

    ptr.reset();
    assert(!ptr);

    ret = mng1.request_scoped_conn(ptr);
    
    cout << "SCOPED END FREE:" << mng1.free_count() << endl;
    assert(mng1.free_count() == 3);

}

assert(mng1.free_count() == 4);

```

More usage can refer the test-cases.   