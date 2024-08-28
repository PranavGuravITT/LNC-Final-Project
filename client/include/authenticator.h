#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include "clientConnection.h"
#include <iostream>
#include <string>

class Authenticator {
public:
    Authenticator(ClientConnection &connection);
    bool authenticate(const std::string &userId, const std::string &password);
private:
    ClientConnection &connection;
};

#endif
