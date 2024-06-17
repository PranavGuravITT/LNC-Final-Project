#include "database.h"

Database::Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
    driver = sql::mysql::get_mysql_driver_instance();
    std::cout<<"database active"<<std::endl;
    connection = driver->connect("tcp://" + host + ":3306", user, password);
    connection->setSchema(database);
}

Database::~Database() {
    delete connection;
}

sql::Connection* Database::getConnection() {
    return connection;
}
