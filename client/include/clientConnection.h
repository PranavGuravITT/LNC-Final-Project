#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <sstream>

class ClientConnection {
private:
    std::string serverAddress;
    int port;
    int clientSocket;
    struct sockaddr_in address;
    const int bufferSize = 1024;

public:
    ClientConnection(const std::string &serverAddress, int port);
    ~ClientConnection();
    void sendRequestToServer(const std::string &message);
    std::string receiveResponse();

};

#endif
