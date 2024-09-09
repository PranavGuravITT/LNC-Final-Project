#include "authenticator.h"

Authenticator::Authenticator(ClientConnection &connection) : connection(connection) {}


bool Authenticator::authenticate(const std::string &userId, const std::string &password) {
    bool status = false;
    std::string message = "VALIDATE:" + userId + ":" + password;
    connection.sendRequestToServer(message);
    std::string response = connection.receiveResponse();
    if (std::strcmp(response, "success") == 0) {
        status = true;
    }
    return status;
}
