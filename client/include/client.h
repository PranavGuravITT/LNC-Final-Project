// Client.hpp

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include "admin.h"
#include "chef.h"
#include "employee.h"
class Client {
public:
    Client(const std::string& server_address, int port);
    ~Client();
    void run();

private:
    std::string serverAddress;
    int port;
    int clientSocket;
    struct sockaddr_in address;
    const int bufferSize = 1024;

    void createSocket();
    void connectToServer();
    bool authenticate(const std::string& user_id, const std::string& password);
    void sendRequestToServer(const std::string& message);
    void viewMenuRequestToServer(const std::string& message);
    void adminScreen();
    void chefScreen();
    void employeeScreen();
    void sendFeedback();
    void viewMenu(std::string& response);
    void getRecommendations(std::string message);
    void storeRecommendations(std::vector<std::string> recommendations);
    void rollOutDailyMenuRequestToServer(const std::string& message);
    void viewDailyMenuRequestToServer();
    void voteForFoodItem();
    void createProfileRequestToServer();
    void inputProfileDetails(std::string &employeeId, std::string &dietaryPreference, std::string &spiceLevel, std::string &cuisineType, std::string &hasSweetTooth);
    void viewNotificationsRequestToServer(const std::string &message);
    void viewDiscardMenuListRequestToServer();
    void handleDiscardMenuOptions();
    void requestFeedbackFromServer(const std::string &foodItemName)
};

#endif
