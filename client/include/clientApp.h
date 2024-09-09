#ifndef CLIENT_APP_H
#define CLIENT_APP_H

#include <vector>
#include "clientConnection.h"
#include "authenticator.h"
#include "admin.h"
#include "chef.h"
#include "employee.h"

#define CHEF_ID "CHF_1"
#define CHEF_USERNAME "chef"
#define CHEF_PASSWORD "Chef123"

#define ADMIN_ID "ADM_1"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "Admin123"

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
