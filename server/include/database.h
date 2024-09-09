#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h> 
#include <cppconn/statement.h>
#include <mysql_driver.h>
#include <vector>
#include <feedback.h>
#include <sstream>
#include "employee.h"
#include "dailyMenu.h"
class Database {

private:
    sql::mysql::MySQL_Driver *driver;
   sql::Connection *connection;
public:
    Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
    ~Database();
    sql::Connection* getConnection();
    bool validateUser(const std::string& userId, const std::string& password);
    void initializeDatabase();
    bool addFoodItemToDatabase(const FoodItem &foodItem); 
    bool addEmployeeToDatabase(Employee employee);
    std::vector<FoodItem> fetchFoodItemsFromDatabase();
    std::string fetchNotificationsFromDatabase();
    bool deleteFoodItemFromDatabase(int foodItemId);
    bool deleteUserFromDatabase(const std::string &userId);
    bool addFeedbackToDatabase(const FeedbackDetails &feedback);
    std::vector<FeedbackDetails> fetchFeedbacksFromDatabase();
    bool addRecommendationToDatabase(const FoodItem &item);
    std::vector<FoodItem> fetchRecommendationsFromDatabase();
    void clearRecommendationTable();
    std::vector<DailyMenu> fetchDailyMenuFromDatabase();
    void clearDailyMenuTable();
    std::string getDiscardMenuList();
    bool incrementVoteCountInDatabase(int foodItemId);
    bool storeEmployeeProfileInDatabase(const std::string &employeeId, const std::string &foodPreference,
                                   const std::string &spiceLevel, const std::string &cuisinePreference,
                                   const std::string &hasSweetTooth);
    std::string fetchFeedbackForAnFoodItem(const std::string &foodItemName);
    bool addFoodItemsToDailyMenu();
    bool storeNotificationInDatabase(const std::string &message);
    void createTablesInDatabase()
};

#endif 