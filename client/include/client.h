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

class Client {
public:
    Client(const std::string& server_address, int port);
    ~Client();
    void run();

private:
    std::string server_address_;
    int port_;
    int client_fd_;
    struct sockaddr_in server_addr_;
    const int buffer_size_ = 1024;

    void create_socket();
    void connect_to_server();
    void authenticate(const std::string& user_id, const std::string& password);
    void storeFoodItem();
    void adminScreen();
    void takeEmployeeInformation();
};

#endif
