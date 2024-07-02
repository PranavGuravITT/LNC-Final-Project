#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "database.h"
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <thread>
#include "foodItem.h"
#include "employee.h"
#include "feedback.h"
#include "server.h"
#include "recommendationEngine.h"
#include "dailyMenu.h"

class Server {
public:
    Server(int port,Database *database);
    ~Server();
    void run();

private:
    int port;
    int serverFD;
    int clientSocket;
    struct sockaddr_in address;
    int addressLength;
    const int bufferSize = 1024;
    std::unordered_map<std::string, std::string> user_db_;
    Database *userDatabase;

    void createSocket();
    void bindSocket();
    void listenForConnections();
    void acceptConnection();
    void handleClient();
    bool validateUser(const std::string& userId, const std::string& password);
    void initializeDatabase();
    bool addFoodItemToDatabase(const FoodItem &foodItem); 
    bool addEmployeeToDatabase(Employee employee);
    std::vector<FoodItem> fetchFoodItemsFromDatabase();
    bool deleteFoodItemFromDatabase(int foodItemId);
    bool deleteUserFromDatabase(const std::string &userId);
    bool addFeedbackToDatabase(const FeedbackDetails &feedback);
    std::vector<FeedbackDetails> fetchFeedbacksFromDatabase();
    bool addRecommendationToDatabase(const FoodItem &item);
    std::vector<FoodItem> fetchRecommendationsFromDatabase();
    void clearRecommendationTable();
    std::vector<DailyMenu> fetchDailyMenuFromDatabase();
    void clearDailyMenuTable();
};

#endif
