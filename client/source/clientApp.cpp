#include "clientApp.h"

ClientApp::ClientApp(const std::string &serverAddress, int port)
    : clientConnection(serverAddress, port) {}

void ClientApp::displayMenu(std::string response)
{
    std::istringstream menuStream(response);
    std::string menuItem;
    std::vector<std::string> menuItems;
    while (std::getline(menuStream, menuItem, '|'))
    {
        menuItems.push_back(menuItem);
    }
    std::cout << "\n\nMenu List:\n";
    for (const auto &item : menuItems)
    {
        std::istringstream itemStream(item);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(itemStream, token, ';'))
        {
            fields.push_back(token);
        }

        if (fields.size() == 5)
        {
            std::cout << "ID: " << fields[0]
                      << "\tName: " << fields[1]
                      << "\t\tPrice: " << fields[2]
                      << "\tAvailability: " << (fields[3] == "1" ? "Yes" : "No") << "\n";
        }
    }
}

void ClientApp::displayDiscardMenuList(std::string response)
{

    std::istringstream discardStream(response);
    std::string discardItem;
    std::vector<std::string> discardItems;
    while (std::getline(discardStream, discardItem, '|'))
    {
        discardItems.push_back(discardItem);
    }

    std::cout << "\n\nDiscard Menu List:\n";
    for (const auto &item : discardItems)
    {
        std::istringstream itemStream(item);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(itemStream, token, ':'))
        {
            fields.push_back(token);
        }

        if (fields.size() == 3)
        {
            std::cout << "ID: " << fields[0]
                      << "\tName: " << fields[1]
                      << "\t\tAverage Rating: " << fields[2] << "\n";
        }
    }
}

void displayRecommendations(std::string response)
{
    std::istringstream recommendationsStream(response);
    std::string recommendation;
    std::vector<std::string> recommendations;
    while (std::getline(recommendationsStream, recommendation, '|'))
    {
        recommendations.push_back(recommendation);
    }
    std::cout << "Recommendations:\n";
    for (const auto &rec : recommendations)
    {
        std::istringstream recommendationStream(rec);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(recommendationStream, token, ';'))
        {
            fields.push_back(token);
        }

        if (fields.size() == 5)
        {
            std::cout << "Food Item: " << fields[1] << "\n";
            std::cout << "Price: " << fields[2] << "\n";
            std::cout << "Rating: " << fields[4] << "\n";
            std::cout << "Availability: " << (fields[3] == "1" ? "Yes" : "No") << "\n";
            std::cout << "------------------\n";
        }
    }
}

void ClientApp::handleDiscardMenuOptions()
{
    char choice;
    std::cout << "1. Remove Food Item from Menu\n";
    std::cout << "2. Give Feedback\n";
    std::cout << "3. Back\n";
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    std::cin.ignore();

    switch (choice)
    {
    case '1':
    {
        int foodItemId;
        std::cout << "Enter Food Item ID to delete: ";
        std::cin >> foodItemId;

        std::string request = "DELETE_FOOD_ITEM:" + std::to_string(foodItemId);
        clientConnection.sendRequestToServer(request);
        break;
    }
    case '2':
    {
        std::string foodItemName, feedback;
        std::cout << "Enter Food Item Name to request feedback for: ";
        std::getline(std::cin, foodItemName);
        std::string message = "We are trying to improve your experience with " + foodItemName +
                              ". Please provide your feedback and help us.\n" +
                              "Q1. What didn’t you like about " + foodItemName + "?\n" +
                              "Q2. How would you like " + foodItemName + " to taste?\n" +
                              "Q3. Share your mom’s recipe.\n";
        std::getline(std::cin, feedback);
        std::string request = "REQUEST_FEEDBACK:" + feedback;
        clientConnection.sendRequestToServer(request);
        break;
    }
    case '3':
        return;
    default:
        std::cout << "Invalid choice. Please try again.\n";
        break;
    }
}

void ClientApp::run()
{
    while (true)
    {
        char choice;
        std::cout << "----------CAFETARIA-------------" << std::endl;
        std::cout << "LOGIN AS" << std::endl;
        std::cout << "1. ADMIN" << std::endl;
        std::cout << "2. CHEF" << std::endl;
        std::cout << "3. EMPLOYEE" << std::endl;
        std::cin >> choice;

        switch (choice)
        {
        case '1':
            adminScreen();
            break;
        case '2':
            chefScreen();
            break;
        case '3':
            employeeScreen();
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            break;
        }
    }
}

void ClientApp::displayNotifications(std::string response)
{
    std::cout << response << std::endl;
}

void ClientApp::displayDailyMenu(std::string &response)
{
    std::istringstream menuStream(response);
    std::string menuItem;
    std::vector<std::string> menuItems;
    while (std::getline(menuStream, menuItem, '|'))
    {
        menuItems.push_back(menuItem);
    }

    std::cout << "\n\nDaily Menu List:\n";
    for (const auto &item : menuItems)
    {
        std::istringstream itemStream(item);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(itemStream, token, ';'))
        {
            fields.push_back(token);
        }

        if (fields.size() == 4)
        {
            std::cout << "ID: " << fields[0]
                      << "\tName: " << fields[1]
                      << "\t\tPrice: " << fields[2] << "\n";
        }
    }
}

void ClientApp::adminScreen()
{
    Authenticator authenticator(clientConnection);
    Admin admin(ADMIN_ID, ADMIN_USERNAME, ADMIN_PASSWORD);

    std::string userId, password, request, response;
    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;

    if (authenticator.authenticate(userId, password) && userId == "ADM_1")
    {
        while (true)
        {
            char choice;
            std::cout << "\n\n----------WELCOME ADMIN-------------" << std::endl;
            std::cout << "1. ADD EMPLOYEE" << std::endl;
            std::cout << "2. ADD FOOD ITEM" << std::endl;
            std::cout << "3. VIEW MENU" << std::endl;
            std::cout << "4. DELETE USER" << std::endl;
            std::cout << "5. DELETE FOOD ITEM" << std::endl;
            std::cout << "6. VIEW DISCARD MENU LIST" << std::endl;
            std::cout << "7. LOG OUT" << std::endl;
            std::cin >> choice;

            switch (choice)
            {
            case '1':
                request = admin.addEmployeeRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '2':
                request = admin.addFoodItemToMenuRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '3':
                request = admin.viewMenuRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayMenu(response);
                break;
            case '4':
                request = admin.deleteEmployeeRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '5':
                request = admin.deleteFoodItemRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '6':
                request = admin.viewDiscardMenuListRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayDiscardMenuList(response);
                handleDiscardMenuOptions();
                break;
            case '7':
                return;
            default:
                std::cout << "Invalid choice" << std::endl;
                break;
            }
        }
    }
}

void ClientApp::chefScreen()
{
    Authenticator authenticator(clientConnection);
     Chef chef(CHEF_ID, CHEF_USERNAME, CHEF_PASSWORD);

    std::string userId, password, request, response;

    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;

    if (authenticator.authenticate(userId, password) && userId == "CHF_1")
    {
        while (1)
        {
            char choice;

            std::cout << "\n\n----------WELCOME CHEF-------------" << std::endl;
            std::cout << "\n-------OPERATIONS-----" << std::endl;
            std::cout << "1.GET RECOMMENDATIONS" << std::endl;
            std::cout << "2.ROLL OUT MENU" << std::endl;
            std::cout << "3.VIEW MENU" << std::endl;
            std::cout << "4.VIEW NOTIFICATIONS" << std::endl;
            std::cout << "5.LOG OUT" << std::endl;
            std::cin >> choice;

            switch (choice)
            {
            case '1':
                request = chef.getReccommendationRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayRecommendations(response);
                break;
            case '2':
                request = chef.rollOutMenuForNextDay();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '3':
                request = chef.viewMenuRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayMenu(response);
                break;
            case '4':
                request = chef.checkNotificationsRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayNotifications(response);
                break;
            case '5':
                return;
            default:
                break;
            }
        }
    }
}

void ClientApp::employeeScreen()
{
    std::cout << "\n\n----------WELCOME EMPLOYEE------------" << std::endl;
    Authenticator authenticator(clientConnection);
    std::string userId, password, request, response;
    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;
    Employee employee;
    if (authenticator.authenticate(userId, password))
    {
        while (1)
        {
            char choice;
            std::cout << "\n-------OPERATIONS-----" << std::endl;
            std::cout << "1.VIEW DAILY MENU" << std::endl;
            std::cout << "2.GIVE FEEDBACK" << std::endl;
            std::cout << "3.VOTE " << std::endl;
            std::cout << "4.CREATE PROFILE " << std::endl;
            std::cout << "5.LOG OUT" << std::endl;
            std::cin >> choice;

            switch (choice)
            {
            case '1':
                request = employee.viewDailyMenuRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                displayDailyMenu(response);
                break;
            case '2':
                request = employee.feedbackRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '3':
                request = employee.voteforFoodItemRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '4':
                request = employee.createProfileRequest();
                clientConnection.sendRequestToServer(request);
                response = clientConnection.receiveResponse();
                break;
            case '5':
                return;
            default:
                break;
            }
        }
    }
}
