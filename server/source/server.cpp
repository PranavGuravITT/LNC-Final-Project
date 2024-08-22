#include "server.h"
#include "feedback.h"

CommandType Server::getCommandType(const std::string &command)
{
    if (command == "VALIDATE")
        return VALIDATE;
    if (command == "REGISTER_EMPLOYEE")
        return REGISTER_EMPLOYEE;
    if (command == "ADD_FOOD_ITEM")
        return ADD_FOOD_ITEM;
    if (command == "VIEW_MENU")
        return VIEW_MENU;
    if (command == "CHECK_NOTIFICATIONS")
        return CHECK_NOTIFICATIONS;
    if (command == "DELETE_FOOD_ITEM")
        return DELETE_FOOD_ITEM;
    if (command == "DELETE_USER")
        return DELETE_USER;
    if (command == "ADD_FEEDBACK")
        return ADD_FEEDBACK;
    if (command == "GET_RECOMMENDATIONS")
        return GET_RECOMMENDATIONS;
    if (command == "VIEW_DISCARD_MENU")
        return VIEW_DISCARD_MENU;
    if (command == "INCREMENT_VOTE")
        return INCREMENT_VOTE;
    if (command == "STORE_RECOMMENDATIONS")
        return STORE_RECOMMENDATIONS;
    if (command == "ROLL_OUT")
        return ROLL_OUT;
    if (command == "CREATE_PROFILE")
        return CREATE_PROFILE;
    if (command == "VIEW_DAILY_MENU")
        return VIEW_DAILY_MENU;
    if (command == "REQUEST_FEEDBACK")
        return REQUEST_FEEDBACK;
    if (command == "FETCH_FEEDBACK")
        return FETCH_FEEDBACK;
    return UNKNOWN_COMMAND;
}

void Server::handleClient()
{
    char buffer[BUFFER_SIZE] = {0};
    while (true)
    {
        read(clientSocket, buffer, BUFFER_SIZE);
        std::string request(buffer);
        std::istringstream ss(request);

        std::string commandStr;
        std::getline(ss, commandStr, ':');

        CommandType command = getCommandType(commandStr);

        std::string response;
        switch (command)
        {
        case VALIDATE:
            response = handleValidateCommand(ss);
            break;
        case REGISTER_EMPLOYEE:
            response = handleRegisterEmployeeCommand(ss);
            break;
        case ADD_FOOD_ITEM:
            response = handleAddFoodItemCommand(ss);
            break;
        case VIEW_MENU:
            response = handleViewMenuCommand();
            break;
        case CHECK_NOTIFICATIONS:
            response = handleCheckNotificationsCommand();
            break;
        case DELETE_FOOD_ITEM:
            response = handleDeleteFoodItemCommand(ss);
            break;
        case DELETE_USER:
            response = handleDeleteUserCommand(ss);
            break;
        case ADD_FEEDBACK:
            response = handleAddFeedbackCommand(ss);
            break;
        case GET_RECOMMENDATIONS:
            response = handleGetRecommendationsCommand();
            break;
        case VIEW_DISCARD_MENU:
            response = handleViewDiscardMenuCommand();
            break;
        case INCREMENT_VOTE:
            response = handleIncrementVoteCommand(ss);
            break;
        case STORE_RECOMMENDATIONS:
            response = handleStoreRecommendationsCommand(ss);
            break;
        case ROLL_OUT:
            response = handleRollOutCommand();
            break;
        case CREATE_PROFILE:
            response = handleCreateProfileCommand(ss);
            break;
        case VIEW_DAILY_MENU:
            response = handleViewDailyMenuCommand();
            break;
        case REQUEST_FEEDBACK:
            response = handleRequestFeedbackCommand(ss);
            break;
        case FETCH_FEEDBACK:
            response = handleFetchFeedbackCommand(ss);
            break;
        default:
            response = "Unknown command";
            break;
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
}
std::string Server::handleValidateCommand(std::istringstream &ss)
{
    std::string userId, password;
    std::getline(ss, userId, ':');
    std::getline(ss, password, ':');
    return database->validateUser(userId, password) ? RESPONSE_SUCCESS : RESPONSE_FAILURE;
}

std::string Server::handleRegisterEmployeeCommand(std::istringstream &ss)
{
    std::string employeeId, employeeName, password;
    std::getline(ss, employeeId, ':');
    std::getline(ss, employeeName, ':');
    std::getline(ss, password, ':');
    Employee employee(employeeId, employeeName, password);
    return database->addEmployeeToDatabase(employee) ? "Employee added successfully" : "Failed to add new employee";
}

std::string Server::handleAddFoodItemCommand(std::istringstream &ss)
{
    std::string foodName, foodType, cuisineType, spiceLevel, response;
    double price;
    std::getline(ss, foodName, ':');
    ss >> price;
    ss.ignore();
    std::getline(ss, foodType, ':');
    std::getline(ss, cuisineType, ':');
    std::getline(ss, spiceLevel, ':');
    FoodItem foodItem;
    foodItem.setFoodItemName(foodName);
    foodItem.setPrice(price);
    foodItem.setFoodType(foodType);
    foodItem.setCuisineType(cuisineType);
    foodItem.setSpiceLevel(spiceLevel);
    foodItem.setAvailability(true);
    if (database->addFoodItemToDatabase(foodItem))
    {
        database->storeNotificationInDatabase("New food item added: " + foodName);
        response = "New food item added";
    }
    else
    {
        response = "Failed to add food item";
    }
    return response;
}

std::string Server::handleViewMenuCommand()
{
    auto foodItems = database->fetchFoodItemsFromDatabase();
    std::string responseData;
    for (const auto &item : foodItems)
    {
        responseData += std::to_string(item.getFoodItemId()) + ";" + item.getFoodItemName() + ";" + std::to_string(item.getPrice()) + ";" + (item.checkAvailability() ? "1" : "0") + ";" + std::to_string(item.getRating()) + "|";
    }

    if (!responseData.empty())
    {
        responseData.pop_back();
    }

    return responseData;
}

std::string Server::handleCheckNotificationsCommand()
{
    return database->fetchNotificationsFromDatabase();
}

std::string Server::handleDeleteFoodItemCommand(std::istringstream &ss)
{
    int foodItemId;
    ss >> foodItemId;
    return database->deleteFoodItemFromDatabase(foodItemId) ? "Food item deleted successfully" : "Failed to delete food item";
}

std::string Server::handleDeleteUserCommand(std::istringstream &ss)
{
    std::string userId;
    std::getline(ss, userId, ':');
    return database->deleteUserFromDatabase(userId) ? "User deleted successfully" : "Failed to delete user";
}

std::string Server::handleAddFeedbackCommand(std::istringstream &ss)
{
    std::string employeeId, comment;
    int foodItemId, rating;
    std::getline(ss, employeeId, ':');
    ss >> foodItemId;
    ss.ignore(1, ':');
    ss >> rating;
    ss.ignore(1, ':');
    std::getline(ss, comment, ':');
    FeedbackDetails feedback(foodItemId, employeeId, rating, comment);
    return database->addFeedbackToDatabase(feedback) ? "Feedback stored successfully" : "Failed to store feedback";
}

std::string Server::handleGetRecommendationsCommand()
{
    auto feedbacks = database->fetchFeedbacksFromDatabase();
    RecommendationEngine engine(database);
    auto recommendationFoodItems = engine.getRecommendations(feedbacks);
    std::string responseData;
    for (const auto &item : recommendationFoodItems)
    {
        responseData += std::to_string(item.getFoodItemId()) + ";" + item.getFoodItemName() + ";" +
                        std::to_string(item.getPrice()) + ";" + (item.checkAvailability() ? "1" : "0") + ";" +
                        std::to_string(item.getRating()) + "|";
    }
    if (!responseData.empty())
        responseData.pop_back();
    return responseData;
}

std::string Server::handleViewDiscardMenuCommand()
{
    return database->getDiscardMenuList();
}

std::string Server::handleIncrementVoteCommand(std::istringstream &ss)
{
    int foodItemId;
    ss >> foodItemId;
    return database->incrementVoteCountInDatabase(foodItemId) ? "Vote count incremented successfully" : "Failed to increment vote count";
}

std::string Server::handleStoreRecommendationsCommand(std::istringstream &ss)
{
    std::string recommendationsStr;
    std::getline(ss, recommendationsStr);
    std::istringstream recommendationsStream(recommendationsStr);
    std::string recommendation;
    database->clearRecommendationTable();
    while (std::getline(recommendationsStream, recommendation, '|'))
    {
        std::istringstream recommendationStream(recommendation);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(recommendationStream, token, ';'))
        {
            fields.push_back(token);
        }
        FoodItem foodItem(fields[1], std::stod(fields[2]));
        foodItem.setFoodItemId(std::stoi(fields[0]));
        foodItem.setAvailability(fields[3] == "1");
        foodItem.setRating(std::stoi(fields[4]));
        database->addRecommendationToDatabase(foodItem);
    }
    return "Recommendations stored successfully";
}

std::string Server::handleCreateProfileCommand(std::istringstream &ss)
{
    std::string userId, profileAnswers;
    std::getline(ss, userId, ':');
    std::getline(ss, profileAnswers);
    std::istringstream profileStream(profileAnswers);
    std::string answer;
    std::vector<std::string> answers;
    while (std::getline(profileStream, answer, ':'))
    {
        answers.push_back(answer);
    }
    std::string foodPreference = answers[0];
    std::string spiceLevel = answers[1];
    std::string cuisinePreference = answers[2];
    std::string sweetTooth = answers[3];
    database->storeEmployeeProfileInDatabase(userId, foodPreference, spiceLevel, cuisinePreference, sweetTooth);
    return "Profile updated successfully";
}

std::string Server::handleRollOutCommand()
{
    database->addFoodItemsToDailyMenu() ? "Menu Rolled Out Successfully" : "Failed to Roll out";
}

std::string Server::handleViewDailyMenuCommand()
{
    auto dailyMenuItems = database->fetchDailyMenuFromDatabase();
    std::string responseData;
    for (const auto &item : dailyMenuItems)
    {
        responseData += std::to_string(item.itemId) + ";" + item.foodItemName + ";" +
                        std::to_string(item.price) + ";" + std::to_string(item.rating) + "|";
    }
    if (!responseData.empty())
        responseData.pop_back();
    return responseData;
}

std::string Server::handleFetchFeedbackCommand(std::istringstream &ss)
{
    std::string foodItemName, responseData;
    std::getline(ss, foodItemName);
    responseData = database->fetchFeedbackForAnFoodItem(foodItemName);
    return responseData;
}

Server::Server(int port, Database *database) : port(port), database(database), serverFD(0), clientSocket(0), addressLength(sizeof(address))
{
    memset(&address, 0, sizeof(address));
    createSocket();
    bindSocket();
    initializeDatabase();
}

Server::~Server()
{
    close(clientSocket);
    close(serverFD);
}

void Server::createSocket()
{
    if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void Server::bindSocket()
{
    int option = 1;
    if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFD, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::listenForConnections()
{
    if (listen(serverFD, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::acceptConnection()
{
    if ((clientSocket = accept(serverFD, (struct sockaddr *)&address, (socklen_t *)&addressLength)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
}

void Server::initializeDatabase()
{
    database->createTablesInDatabase();
}

void Server::run()
{
    listenForConnections();
    while (true)
    {
        acceptConnection();
        std::thread clientRequestThread(&Server::handleClient, this);
        clientRequestThread.detach();
    }
}
