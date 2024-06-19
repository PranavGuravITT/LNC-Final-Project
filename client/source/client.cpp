
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

    std::string message = "ADD_FEEDBACK:EMP_1:4:5";
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Feedback sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::cout << "Response from server: " << buffer << std::endl;
}

void Client::getRecommendations()
{
    std::string message = "GET_RECOMMENDATIONS";

    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Recommendation request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);

    std::string responseStr(buffer);
    std::cout << "Recommendations received:\n";

    std::istringstream recommendationsStream(responseStr);
    std::string recommendation;
    int count = 0;
    std::ostringstream recommendationsToSend;

    while (std::getline(recommendationsStream, recommendation, '|') && count < 5)
    {
        std::istringstream recommendationStream(recommendation);
        std::string token;

        std::vector<std::string> fields;
        while (std::getline(recommendationStream, token, ';'))
        {
            fields.push_back(token);
        }

        if (fields.size() == 5)
        {
            std::cout << "Food Item: " << fields[1] << "\n";
            std::cout << "Price: " << std::stod(fields[2]) << "\n";
            std::cout << "Rating: " << std::stoi(fields[4]) << "\n";
            std::cout << "Availability: " << (fields[3] == "1" ? "Yes" : "No") << "\n";
            std::cout << "------------------\n";

            recommendationsToSend << fields[0] << ";" << fields[1] << ";" << std::stod(fields[2]) << ";" << (fields[3] == "1" ? "Yes" : "No") << ";" << std::stoi(fields[4]);
            recommendationsToSend << "|";

            count++;
        }
    }

    storeRecommendations(recommendationsToSend.str());
}

void Client::storeRecommendations(const std::string &recommendationsStr)
{
    std::cout << "Received String to store: " << recommendationsStr << std::endl;

    std::string message = "STORE_RECOMMENDATIONS:" + recommendationsStr;
    std::cout << message << std::endl;

    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Store recommendations request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::cout << "Response from server: " << buffer << std::endl;
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
    std::cout << "Delete food item request sent\n";

    char response[bufferSize] = {0};
    read(clientSocket, response, bufferSize);
    std::cout << response << std::endl;
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
                getMenuDataRequestToServer(request);
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
                getRecommendations();
                break;
            default:
                break;
            }
        }
    }
}

void Client::chefScreen()
{
    Admin admin("CHF_1", "admin", "Admin123");
    while (1)
    {
        char choice;

        std::cout << "\n\n----------WELCOME CHEF-------------" << std::endl;
        std::cout << "\n-------OPERATIONS-----" << std::endl;
        std::cout << "1. ADD EMPLOYEE" << std::endl;
        std::cout << "2. ADD FOOD ITEM" << std::endl;
        std::cout << "3. VIEW MENU" << std::endl;
        std::cin >> choice;
        switch (choice)
        {
        // case '1':

        //     break;
        // case '2':
        //     storeFoodItem();
        //     break;
        // case '3':
        //     viewMenu();
        //     break;
        default:
            break;
        }
    }
}

void Client::employeeScreen()
{
    while (1)
    {
        char choice;

        std::cout << "\n\n----------WELCOME EMPLOYEE------------" << std::endl;
        std::cout << "\n-------OPERATIONS-----" << std::endl;
        std::cout << "1. ADD EMPLOYEE" << std::endl;
        std::cout << "2. ADD FOOD ITEM" << std::endl;
        std::cout << "3. EMPLOYEE" << std::endl;
        std::cin >> choice;
        switch (choice)
        {
        // case '1':

        //     break;
        // case '2':
        //     storeFoodItem();
        //     break;
        default:
            break;
        }
    }
}

void Client::getMenuDataRequestToServer(const std::string &message)
{
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "View menu request sent\n";

    char buffer[bufferSize] = {0};
    read(clientSocket, buffer, bufferSize);
    std::string response(buffer);

    Json::CharReaderBuilder reader;
    Json::Value jsonData;
    std::string errors;
    std::istringstream stream(response);
    if (Json::parseFromStream(reader, stream, &jsonData, &errors))
    {
        std::cout << "\n\nMenu List:\n";
        for (const auto &item : jsonData)
        {
            std::cout << "ID: " << item["food_item_id"].asInt()
                      << "\tName: " << item["food_item_name"].asString()
                      << "\t\tPrice: " << item["price"].asDouble()
                      << "\tAvailability: " << (item["availability"].asBool() ? "Yes" : "No") << "\n";
        }
    }
    else
    {
        std::cerr << "Failed to parse the menu: " << errors << std::endl;
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
        std::cout << "Enter User ID: ";
        std::cin >> userId;
        std::cout << "Enter Password: ";
        std::cin >> password;
        if(authenticate(userId, password)){
            employeeScreen();
        }
            break;
        default:
            break;
        }
    }
}
