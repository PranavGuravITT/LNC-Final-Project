

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
#include "foodItem.h"
#include "employee.h"
#include "feedback.h"
#include "recommendationEngine.h"
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>

class Server {
public:
    Server(int port,Database *database);
    ~Server();
    void run();

private:
    int port_;
    int server_fd_;
    int client_socket_;
    struct sockaddr_in address_;
    int addrlen_;
    const int buffer_size_ = 1024;
    std::unordered_map<std::string, std::string> user_db_;
    Database *userDatabase;

    void create_socket();
    void bind_socket();
    void listen_for_connections();
    void accept_connection();
    void handleClient();
    bool validate_user(const std::string& user_id, const std::string& password);
    void initializeDatabase();
    bool addFoodItemToDatabase(const FoodItem &foodItem); 
    bool addEmployeeToDatabase(Employee employee);
    std::vector<FoodItem> fetchFoodItemsFromDatabase();
    bool deleteFoodItemFromDatabase(int foodItemId);
    bool deleteUserFromDatabase(const std::string &user_id);
    bool addFeedbackToDatabase(const FeedbackDetails &feedback);
    std::vector<FeedbackDetails> fetchFeedbacksFromDatabase();
    bool addRecommendationToDatabase(const FoodItem &item);
    std::vector<FoodItem> fetchRecommendationsFromDatabase();
};

#endif
