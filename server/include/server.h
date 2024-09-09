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
#include <thread>
#include "foodItem.h"
#include "employee.h"
#include "feedback.h"
#include "server.h"
#include "recommendationEngine.h"
#include "dailyMenu.h"

#define BUFFER_SIZE 1024
#define RESPONSE_SUCCESS "success"
#define RESPONSE_FAILURE "failure"

enum CommandType {
    VALIDATE,
    REGISTER_EMPLOYEE,
    ADD_FOOD_ITEM,
    VIEW_MENU,
    CHECK_NOTIFICATIONS,
    DELETE_FOOD_ITEM,
    DELETE_USER,
    ADD_FEEDBACK,
    GET_RECOMMENDATIONS,
    VIEW_DISCARD_MENU,
    INCREMENT_VOTE,
    STORE_RECOMMENDATIONS,
    ROLL_OUT,
    CREATE_PROFILE,
    VIEW_DAILY_MENU,
    REQUEST_FEEDBACK,
    FETCH_FEEDBACK,
    UNKNOWN_COMMAND
};
class Server {
    public:
    Server(int port,Database *database);
    ~Server();
    void handleClient();
    void run();
private:
    CommandType getCommandType(const std::string& command);
    std::string handleValidateCommand(std::istringstream& data);
    std::string handleRegisterEmployeeCommand(std::istringstream& data);
    std::string handleAddFoodItemCommand(std::istringstream& data);
    std::string handleViewMenuCommand();
    std::string handleCheckNotificationsCommand();
    std::string handleDeleteFoodItemCommand(std::istringstream& data);
    std::string handleDeleteUserCommand(std::istringstream& data);
    std::string handleAddFeedbackCommand(std::istringstream& data);
    std::string handleGetRecommendationsCommand();
    std::string handleViewDiscardMenuCommand();
    std::string handleIncrementVoteCommand(std::istringstream& data);
    std::string handleStoreRecommendationsCommand(std::istringstream& data);
    std::string handleRollOutCommand();
    std::string handleCreateProfileCommand(std::istringstream& data);
    std::string handleViewDailyMenuCommand();
    std::string handleRequestFeedbackCommand(std::istringstream& data);
    std::string handleFetchFeedbackCommand(std::istringstream& data);

    void createSocket();
    void bindSocket();
    void listenForConnections();
    void acceptConnection();
    void handleClient();
    void initializeDatabase();

    int port;
    int serverFD;
    int clientSocket;
    struct sockaddr_in address;
    int addressLength;
    const int bufferSize = 1024;
    std::unordered_map<std::string, std::string> user_db_;
    Database *database;
};

#endif
