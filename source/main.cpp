#include <iostream>

#include "aisql_conns_manage.hpp"

using namespace aisqlpp::db_config;

int main(int argc, char* argv[])
{
    aisqlpp::conns_manage mng(3, host, user, passwd, db); 

    return 0;
}
