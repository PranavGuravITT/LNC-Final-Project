#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h> 
#include <cppconn/statement.h>
#include <mysql_driver.h>

class Database {
public:
    Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
    ~Database();
    sql::Connection* getConnection();

private:
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *connection;
};

#endif // DATABASE_H
