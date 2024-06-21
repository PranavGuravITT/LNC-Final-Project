
#include "client.h"

Client::Client(const std::string &server_address, int port)
    : server_address_(server_address), port_(port), clientSocket(0)
{
    memset(&server_addr_, 0, sizeof(server_addr_));
    create_socket();
    connect_to_server();
}

Client::~Client()
{
    close(clientSocket);
}

void Client::create_socket()
{
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void Client::connect_to_server()
{
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_address_.c_str(), &server_addr_.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr *)&server_addr_, sizeof(server_addr_)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

void Client::sendFeedback()
{
    int foodItemId;
    int rating;
    std::string employeeId;
    std::string comment;

    std::cout << "Enter Food Item ID: ";
    std::cin >> foodItemId;
    std::cin.ignore();
    std::cout << "Enter Employee ID: ";
    std::getline(std::cin, employeeId);

    std::cout << "Enter Rating(0-5): ";
    std::cin >> rating;
    std::cin.ignore();

    std::cout << "Enter Comment: ";
    std::getline(std::cin, comment);
    std::string message = "ADD_FEEDBACK:" + employeeId + ":" + std::to_string(foodItemId) + ":" + std::to_string(rating) + ":" + comment;
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Feedback sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::cout << "Response from server: " << buffer << std::endl;
}

void Client::getRecommendations(std::string message)
{
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Recommendation request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);

    std::string response(buffer);

    std::istringstream recommendationsStream(response);
    std::string recommendation;
    std::vector<std::string> recommendations;
    while (std::getline(recommendationsStream, recommendation, '|'))
    {
        recommendations.push_back(recommendation);
    }

    std::cout << "Recommendations received:\n";
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

    // Store only the first 5 elements in the database
    storeRecommendations(recommendations);
}

void Client::storeRecommendations(std::vector<std::string> recommendations)
{
    std::string message = "STORE_RECOMMENDATIONS:";
    int count = 0;
    for (const auto &rec : recommendations)
    {
        if (count >= 5)
            break;
        message += rec + "|";
        count++;
    }

    if (!message.empty() && message.back() == '|')
    {
        message.pop_back(); // Remove the last '|'
    }

    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Store request sent" << std::endl;

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
}

bool Client::authenticate(const std::string &userId, const std::string &password)
{
    bool status = false;
    std::string message = "VALIDATE:" + userId + ":" + password;
    std::cout << message << std::endl;
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Authentication request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::cout << "Response from server: " << buffer << std::endl;
    if (!strcmp(buffer, "success"))
    {
        status = true;
    }
    return status;
}

void Client::sendRequestToServer(const std::string &message)
{

    send(clientSocket, message.c_str(), message.length(), 0);

    char response[bufferSize] = {0};
    read(clientSocket, response, bufferSize);
    std::cout << response << std::endl;
}

void Client::rollOutDailyMenuRequestToServer(const std::string &message)
{
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Roll Out menu request sent\n";

    char response[bufferSize] = {0};
    read(clientSocket, response, bufferSize);
    std::cout << response << std::endl;
}

void Client::viewDailyMenuRequestToServer()
{
    std::string message = "VIEW_DAILY_MENU";
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Roll Out menu request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);

    std::string response(buffer);

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
        std::cout << "Vote " << std::endl;
    }
}

void Client::adminScreen()
{
    Admin admin("ADM_1", "admin", "Admin123");
    std::string userId, password;
    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;
    if (authenticate(userId, password) && userId == "ADM_1")
    {
        while (true)
        {
            char choice;

            std::cout << "\n\n----------WELCOME ADMIN-------------" << std::endl;
            std::cout << "\n-------OPERATIONS-----\n"
                      << std::endl;
            std::cout << "1. ADD EMPLOYEE" << std::endl;
            std::cout << "2. ADD FOOD ITEM" << std::endl;
            std::cout << "3. VIEW MENU" << std::endl;
            std::cout << "4. DELETE USER" << std::endl;
            std::cout << "5. DELETE FOOD ITEM" << std::endl;
            std::cout << "6. ADD FEEDDBACK" << std::endl;
            std::cout << "7. RECOMMENDATION ENGINE" << std::endl;
            std::cout << "8. BACK" << std::endl;
            std::cin >> choice;
            std::string request;
            switch (choice)
            {
            case '1':
                request = admin.addEmployeeRequest();
                sendRequestToServer(request);
                break;
            case '2':
                request = admin.addFoodItemToMenuRequest();
                sendRequestToServer(request);
                break;
            case '3':
                request = admin.viewMenuRequest();
                viewMenuRequestToServer(request);
                break;
            case '4':
                request = admin.deleteEmployeeRequest();
                sendRequestToServer(request);
                break;
            case '5':
                request = admin.deleteFoodItemRequest();
                sendRequestToServer(request);
                break;
            case '6':
                sendFeedback();
                break;
            case '7':
                getRecommendations("GET_RECOMMENDATIONS");
                break;
            case '8':
                return;
            default:
                break;
            }
        }
    }
}

void Client::chefScreen()
{
    Chef chef("CHF_1", "chef", "Chef123");
    std::string userId, password;
    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;
    if (authenticate(userId, password) && userId == "CHF_1")
    {
        while (1)
        {
            char choice;

            std::cout << "\n\n----------WELCOME CHEF-------------" << std::endl;
            std::cout << "\n-------OPERATIONS-----" << std::endl;
            std::cout << "1.ROLL OUT MENU" << std::endl;
            std::cout << "2.VIEW MENU" << std::endl;
            std::cout << "3.LOG OUT" << std::endl;
            std::cin >> choice;

            std::string request;
            switch (choice)
            {
            case '1':
                request = chef.rollOutMenuForNextDay();
                rollOutDailyMenuRequestToServer(request);

                break;
            case '2':
                request = chef.viewMenuRequest();
                viewMenuRequestToServer(request);
                break;
            case '3':
                return;
            default:
                break;
            }
        }
    }
}

void Client::employeeScreen()
{
    std::cout << "\n\n----------WELCOME EMPLOYEE------------" << std::endl;
    // Employee employee();
    std::string userId, password;
    std::cout << "Enter User ID: ";
    std::cin >> userId;
    std::cout << "Enter Password: ";
    std::cin >> password;
    if (authenticate(userId, password))
    {
        while (1)
        {
            char choice;
            std::cout << "\n-------OPERATIONS-----" << std::endl;
            std::cout << "1.VIEW DAILY MENU" << std::endl;
            std::cout << "2.GIVE FEEDBACK" << std::endl;
            std::cout << "3.LOG OUT" << std::endl;
            std::cin >> choice;

            std::string request;
            switch (choice)
            {
            case '1':
                viewDailyMenuRequestToServer();
                break;
            case '2':
                sendFeedback();
                break;
            default:
                break;
            }
        }
    }
}

void Client::viewMenuRequestToServer(const std::string &message)
{
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "View menu request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::string response(buffer);

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

void Client::run()
{
    while (1)
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
        // if(authenticate(userId, password)){
        //     employeeScreen();
        // }
        //     break;
        default:
            break;
        }
    }
}
