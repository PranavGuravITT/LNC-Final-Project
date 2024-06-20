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
class Client {
public:
    Client(const std::string& server_address, int port);
    ~Client();
    void run();

private:
    std::string server_address_;
    int port_;
    int clientSocket;
    struct sockaddr_in server_addr_;
    const int bufferSize = 1024;

    void create_socket();
    void connect_to_server();
    bool authenticate(const std::string& user_id, const std::string& password);
    void sendRequestToServer(const std::string& message);
    void viewMenuRequestToServer(const std::string& message);
    void adminScreen();
    void chefScreen();
    void employeeScreen();
    void sendFeedback();
    void getRecommendations();
    void storeRecommendations(std::vector<std::string> recommendations);
};

#endif
