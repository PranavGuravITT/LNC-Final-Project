// Server.hpp

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
#include "foodItem.h"

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
    void handle_client();
    bool validate_user(const std::string& user_id, const std::string& password);
    void initialize_user_db();
    bool addFoodItemToDatabase(const FoodItem &foodItem); 
};

#endif
