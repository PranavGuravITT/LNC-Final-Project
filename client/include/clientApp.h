#ifndef CLIENT_APP_H
#define CLIENT_APP_H

#include <vector>
#include "clientConnection.h"
#include "authenticator.h"
#include "admin.h"
#include "chef.h"
#include "employee.h"

class ClientApp {
private:
    ClientConnection clientConnection;

public:
    ClientApp(const std::string &serverAddress, int port);
    void run();
    void adminScreen();
    void chefScreen();
    void employeeScreen();
    void displayMenu(std::string menuItems);
    void displayDiscardMenuList(std::string discardItems);
    void displayRecommendations(std::string response);
    void handleDiscardMenuOptions();
    void displayNotifications(std::string response);
    void displayDailyMenu(std::string& response)
};

#endif
