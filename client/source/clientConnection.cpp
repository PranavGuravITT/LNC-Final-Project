#include "clientConnection.h"

ClientConnection::ClientConnection(const std::string &serverAddress, int port)
    : serverAddress(serverAddress), port(port) {
    memset(&address, 0, sizeof(address));
    
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, serverAddress.c_str(), &address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

ClientConnection::~ClientConnection() {
    close(clientSocket);
}


void ClientConnection::sendRequestToServer(const std::string &message) {
    send(clientSocket, message.c_str(), message.length(), 0);
}

std::string ClientConnection::receiveResponse() {
    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    return std::string(buffer);
}