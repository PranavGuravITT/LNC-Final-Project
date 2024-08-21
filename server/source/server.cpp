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
    return userDatabase->validateUser(userId, password) ? RESPONSE_SUCCESS : RESPONSE_FAILURE;
}

std::string Server::handleRegisterEmployeeCommand(std::istringstream &ss)
{
    std::string employeeId, employeeName, password;
    std::getline(ss, employeeId, ':');
    std::getline(ss, employeeName, ':');
    std::getline(ss, password, ':');
    Employee employee(employeeId, employeeName, password);
    return userDatabase->addEmployeeToDatabase(employee) ? "Employee added successfully" : "Failed to add new employee";
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
    if (userDatabase->addFoodItemToDatabase(foodItem))
    {
        sendNotificationToChef("New food item added: " + foodName);
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
    auto foodItems = userDatabase->fetchFoodItemsFromDatabase();
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

std::string Server::handleCheckNotificationsCommand() {
    return userDatabase->fetchNotificationsFromDatabase();
}

std::string Server::handleDeleteFoodItemCommand(std::istringstream& ss) {
    int foodItemId;
    ss >> foodItemId;
    return userDatabase->deleteFoodItemFromDatabase(foodItemId) ? "Food item deleted successfully" : "Failed to delete food item";
}

std::string Server::handleDeleteUserCommand(std::istringstream& ss) {
    std::string userId;
    std::getline(ss, userId, ':');
    return userDatabase->deleteUserFromDatabase(userId) ? "User deleted successfully" : "Failed to delete user";
}

std::string Server::handleAddFeedbackCommand(std::istringstream& ss) {
    std::string employeeId, comment;
    int foodItemId, rating;
    std::getline(ss, employeeId, ':');
    ss >> foodItemId;
    ss.ignore(1, ':');
    ss >> rating;
    ss.ignore(1, ':');
    std::getline(ss, comment, ':');
    FeedbackDetails feedback(foodItemId, employeeId, rating, comment);
    return userDatabase->addFeedbackToDatabase(feedback) ? "Feedback stored successfully" : "Failed to store feedback";
}

std::string Server::handleGetRecommendationsCommand() {
    auto feedbacks = userDatabase->fetchFeedbacksFromDatabase();
    RecommendationEngine engine(userDatabase);
    auto recommendationFoodItems = engine.getRecommendations(feedbacks);
    std::string responseData;
    for (const auto& item : recommendationFoodItems) {
        responseData += std::to_string(item.getFoodItemId()) + ";" + item.getFoodItemName() + ";" +
                        std::to_string(item.getPrice()) + ";" + (item.checkAvailability() ? "1" : "0") + ";" +
                        std::to_string(item.getRating()) + "|";
    }
    if (!responseData.empty()) responseData.pop_back();
    return responseData;
}

std::string Server::handleViewDiscardMenuCommand() {
    return userDatabase->getDiscardMenuList();
}

std::string Server::handleIncrementVoteCommand(std::istringstream& ss) {
    int foodItemId;
    ss >> foodItemId;
    return userDatabase->incrementVoteCountInDatabase(foodItemId) ? "Vote count incremented successfully" : "Failed to increment vote count";
}

std::string Server::handleStoreRecommendationsCommand(std::istringstream& ss) {
    std::string recommendationsStr;
    std::getline(ss, recommendationsStr);
    std::istringstream recommendationsStream(recommendationsStr);
    std::string recommendation;
    userDatabase->clearRecommendationTable();
    while (std::getline(recommendationsStream, recommendation, '|')) {
        std::istringstream recommendationStream(recommendation);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(recommendationStream, token, ';')) {
            fields.push_back(token);
        }
        FoodItem foodItem(fields[1], std::stod(fields[2]));
        foodItem.setFoodItemId(std::stoi(fields[0]));
        foodItem.setAvailability(fields[3] == "1");
        foodItem.setRating(std::stoi(fields[4]));
        userDatabase->addRecommendationToDatabase(foodItem);
    }
    return "Recommendations stored successfully";
}

std::string Server::handleCreateProfileCommand(std::istringstream& ss) {
    std::string userId, profileAnswers;
    std::getline(ss, userId, ':');
    std::getline(ss, profileAnswers);
    std::istringstream profileStream(profileAnswers);
    std::string answer;
    std::vector<std::string> answers;
    while (std::getline(profileStream, answer, ':')) {
        answers.push_back(answer);
    }
    std::string foodPreference = answers[0];
    std::string spiceLevel = answers[1];
    std::string cuisinePreference = answers[2];
    std::string sweetTooth = answers[3];
    userDatabase->storeEmployeeProfileInDatabase(userId, foodPreference, spiceLevel, cuisinePreference, sweetTooth);
    return "Profile updated successfully";
}

std::string Server::handleRollOutCommand(){
    userDatabase->addFoodItemsToDailyMenu() ? "Menu Rolled Out Successfully" : "Failed to Roll out";
}
Server::Server(int port, Database *database) : port(port), userDatabase(database), serverFD(0), clientSocket(0), addressLength(sizeof(address))
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
    std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
    stmt->execute("CREATE TABLE IF NOT EXISTS EMPLOYEES ("
                  "employee_id VARCHAR(255) PRIMARY KEY, "
                  "employee_name VARCHAR(255) NOT NULL UNIQUE, "
                  "password VARCHAR(255) NOT NULL)");

    stmt->execute("CREATE TABLE IF NOT EXISTS FOOD_ITEMS ("
                  "food_item_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "food_item_name VARCHAR(100) NOT NULL, "
                  "price DOUBLE NOT NULL, "
                  "availability BOOLEAN NOT NULL, "
                  "food_type ENUM('Vegetarian', 'NonVegetarian', 'Eggetarian') NOT NULL, "
                  "cuisine_type ENUM('NorthIndian', 'SouthIndian', 'Other') NOT NULL, "
                  "spice_level ENUM('High', 'Medium', 'Low') NOT NULL)");

    stmt->execute("CREATE TABLE IF NOT EXISTS FEEDBACK ("
                  "feedback_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "food_item_id INT, "
                  "employee_id VARCHAR(255), "
                  "rating INT, "
                  "comment VARCHAR(255), "
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id) ON DELETE CASCADE, "
                  "FOREIGN KEY (employee_id) REFERENCES EMPLOYEES(employee_id) ON DELETE CASCADE)");

    stmt->execute("CREATE TABLE IF NOT EXISTS VOTES ("
                  "food_item_id INT PRIMARY KEY, "
                  "count_of_votes INT NOT NULL, "
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id) ON DELETE CASCADE)");

    stmt->execute("CREATE TABLE IF NOT EXISTS RECOMMENDATIONS ("
                  "id INT AUTO_INCREMENT PRIMARY KEY, "
                  "food_item_id INT NOT NULL, "
                  "food_item_name VARCHAR(255) NOT NULL, "
                  "price DOUBLE NOT NULL, "
                  "rating INT NOT NULL, "
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id) ON DELETE CASCADE)");

    stmt->execute("CREATE TABLE IF NOT EXISTS DAILY_MENU ("
                  "menu_date DATE NOT NULL, "
                  "item_id INT NOT NULL, "
                  "PRIMARY KEY (item_id, menu_date), "
                  "FOREIGN KEY (item_id) REFERENCES RECOMMENDATIONS(food_item_id) ON DELETE CASCADE)");

    stmt->execute("DROP TRIGGER IF EXISTS before_insert_daily_menu");

    stmt->execute("CREATE TRIGGER before_insert_daily_menu "
                  "BEFORE INSERT ON DAILY_MENU "
                  "FOR EACH ROW "
                  "BEGIN "
                  "IF NEW.menu_date IS NULL THEN "
                  "SET NEW.menu_date = CURRENT_DATE(); "
                  "END IF; "
                  "END;");

    stmt->execute("CREATE TABLE IF NOT EXISTS EMPLOYEE_PROFILES ("
                  "profile_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "employee_id VARCHAR(255) UNIQUE, "
                  "food_preference ENUM('Vegetarian', 'Non Vegetarian', 'Eggetarian'), "
                  "spice_level ENUM('High', 'Medium', 'Low'), "
                  "cuisine_preference ENUM('North Indian', 'South Indian', 'Other'), "
                  "sweet_tooth ENUM('No', 'Yes'), "
                  "FOREIGN KEY (employee_id) REFERENCES EMPLOYEES(employee_id) ON DELETE CASCADE)");

    stmt->execute("CREATE TABLE IF NOT EXISTS NOTIFICATIONS ("
                  "notification_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "message VARCHAR(255) NOT NULL, "
                  "recipient VARCHAR(255) NOT NULL)");

    stmt->execute("CREATE TABLE IF NOT EXISTS DISCARD_MENU ("
                  "food_item_id INT PRIMARY KEY, "
                  "food_item_name VARCHAR(255) NOT NULL, "
                  "avg_rating DOUBLE NOT NULL, "
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id) ON DELETE CASCADE)");

    stmt->execute("CREATE TABLE IF NOT EXISTS FEEDBACK_RESPONSES ("
                  "response_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "food_item_name VARCHAR(100) NOT NULL, "
                  "feedback_text TEXT NOT NULL)");
}



void Server::handleDiscardMenuOptions(const std::string &command, const std::string &data)
{
    if (command == "REMOVE_ITEM")
    {
        std::string foodItemName;
        std::cout << "Enter Food Item Name to remove: ";
        std::getline(std::cin, foodItemName);

        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "DELETE FROM FOOD_ITEMS WHERE food_item_name = ?"));
        pstmt->setString(1, foodItemName);
        pstmt->execute();

        std::cout << "Food item removed from menu.\n";
    }
    else if (command == "REQUEST_FEEDBACK")
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO FEEDBACK_RESPONSES (food_item_name, feedback_text) VALUES (?, ?)"));
        pstmt->setString(1, data);
        pstmt->setString(2, data);
        pstmt->execute();
        std::cout << "Feedback received and stored in the database.\n";
    }
}

std::string Server::handleViewDailyMenuCommand() {
    auto dailyMenuItems = userDatabase->fetchDailyMenuFromDatabase();
    std::string responseData;
    for (const auto& item : dailyMenuItems) {
        responseData += std::to_string(item.itemId) + ";" + item.foodItemName + ";" +
                        std::to_string(item.price) + ";" + std::to_string(item.rating) + "|";
    }
    if (!responseData.empty()) responseData.pop_back();
    return responseData;
}

std::string Server::handleFetchFeedbackCommand(std::istringstream& ss) {
    std::string foodItemName,responseData;
    std::getline(ss, foodItemName);
    responseData = userDatabase->fetchFeedbackForAnFoodItem(foodItemName);
    return responseData;
}



void Server::sendNotificationToChef(const std::string &message)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO NOTIFICATIONS (message, recipient) VALUES (?, 'chef')"));

        pstmt->setString(1, message);
        pstmt->execute();
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
}

std::string Server::checkNotificationsForChef()
{
    std::string notifications;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT message FROM NOTIFICATIONS WHERE recipient = 'chef'"));

        while (res->next())
        {
            notifications += res->getString("message") + "\n";
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return notifications;
}
void Server::fetchFeedbackForAnFoodItem(const std::string &foodItemName)
{
    std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
        "SELECT feedback_text FROM FEEDBACK_RESPONSES WHERE food_item_name = ?"));
    pstmt->setString(1, foodItemName);
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    std::string feedbackList;
    while (res->next())
    {
        feedbackList += res->getString("feedback_text") + "|";
    }

    send(clientSocket, feedbackList.c_str(), feedbackList.length(), 0);
}

void Server::handleClient()
{
    char buffer[bufferSize] = {0};
    while (true)
    {
        read(clientSocket, buffer, bufferSize);
        std::string request(buffer);
        std::cout << "Request " << request << std::endl;
        std::istringstream ss(request);
        std::string command;
        std::getline(ss, command, ':');

        std::cout << "Command received " << command << std::endl;

        std::string response;

        if (command == "VALIDATE")
        {
            std::cout << "Validate" << std::endl;
            std::string userId, password;
            std::getline(ss, userId, ':');
            std::getline(ss, password, ':');
            std::cout << userId << "  " << password << std::endl;
            if (validateUser(userId, password))
            {
                response = "success";
            }
            else
            {
                response = "failed";
            }
        }
        else if (command == "REGISTER_EMPLOYEE")
        {
            std::cout << "Registering employee" << std::endl;
            std::string employeeName;
            std::string employeeId;
            std::string password;
            std::getline(ss, employeeId, ':');
            std::getline(ss, employeeName, ':');
            std::getline(ss, password, ':');
            std::cout << employeeId << employeeName << password << std::endl;
            Employee employee(employeeId, employeeName, password);
            if (addEmployeeToDatabase(employee))
            {
                response = "Employee added successfully";
            }
            else
            {
                response = "Failed to add new employee";
            }
        }
        else if (command == "ADD_FOOD_ITEM")
        {
            std::cout << "Adding food item" << std::endl;
            double price;
            bool availability;
            std::string foodName, foodType, cuisineType, spiceLevel;
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

            if (addFoodItemToDatabase(foodItem))
            {
                response = "Food item added successfully";
                sendNotificationToChef("New food item added: " + foodName);
            }
            else
            {
                response = "Failed to add food item";
            }
        }

        else if (command == "VIEW_MENU")
        {
            std::cout << "Viewing menu" << std::endl;
            auto foodItems = fetchFoodItemsFromDatabase();

            std::string responseData;
            for (const auto &item : foodItems)
            {
                responseData += std::to_string(item.getFoodItemId()) + ";" + item.getFoodItemName() + ";" + std::to_string(item.getPrice()) + ";" + (item.checkAvailability() ? "1" : "0") + ";" + std::to_string(item.getRating()) + "|";
            }

            if (!responseData.empty())
            {
                responseData.pop_back();
            }

            response = responseData;
        }
        else if (command == "CHECK_NOTIFICATIONS")
        {
            response = checkNotificationsForChef();
        }

        else if (command == "DELETE_FOOD_ITEM")
        {
            std::cout << "Deleting food item" << std::endl;
            int food_item_id;
            ss >> food_item_id;
            if (deleteFoodItemFromDatabase(food_item_id))
            {
                response = "Food item deleted successfully";
            }
            else
            {
                response = "Failed to delete food item";
            }
        }
        else if (command == "DELETE_USER")
        {
            std::cout << "Deleting user" << std::endl;
            std::string userId;
            std::getline(ss, userId, ':');
            if (deleteUserFromDatabase(userId))
            {
                response = "User deleted successfully";
            }
            else
            {
                response = "Failed to delete user";
            }
        }
        else if (command == "ADD_FEEDBACK")
        {
            std::string employeeId, comment;
            int foodItemId, rating;
            std::getline(ss, employeeId, ':');
            ss >> foodItemId;
            ss.ignore(1, ':');
            ss >> rating;
            ss.ignore(1, ':');
            std::getline(ss, comment, ':');

            std::cout << "Adding feedback" << std::endl;
            std::cout << employeeId << "  " << rating << "  " << foodItemId << "  " << comment << std::endl;
            FeedbackDetails feedback(foodItemId, employeeId, rating, comment);
            std::cout << "Adding feedback2" << std::endl;
            if (addFeedbackToDatabase(feedback))
            {
                response = "Feedback stored successfully";
            }
            else
            {
                response = "Failed to store feedback";
            }
        }
        else if (command == "GET_RECOMMENDATIONS")
        {
            std::cout << "Fetching recommendations" << std::endl;

            auto feedbacks = fetchFeedbacksFromDatabase();
            RecommendationEngine engine(userDatabase);
            auto recommendationFoodItems = engine.getRecommendations(feedbacks);

            std::string responseData;
            for (const auto &item : recommendationFoodItems)
            {
                responseData += std::to_string(item.getFoodItemId()) + ";" + item.getFoodItemName() + ";" + std::to_string(item.getPrice()) + ";" + (item.checkAvailability() ? "1" : "0") + ";" + std::to_string(item.getRating()) + "|";
            }

            if (!responseData.empty())
            {
                responseData.pop_back();
            }

            response = responseData;
        }
        else if (command == "VIEW_DISCARD_MENU")
        {

            response = generateDiscardMenuList();
        }
        else if (command == "INCREMENT_VOTE")
        {
            std::cout << "Incrementing vote count" << std::endl;
            
            if (incrementVoteCount(foodItemId))
            {
                response = "Vote count incremented successfully";
            }
            else
            {
                response = "Failed to increment vote count";
            }
        }
        else if (command == "STORE_RECOMMENDATIONS")
        {
            std::string recommendationsStr;
            std::getline(ss, recommendationsStr);

            std::cout << "Received String: " << recommendationsStr << std::endl;

            std::istringstream recommendationsStream(recommendationsStr);
            std::string recommendation;

            clearRecommendationTable();
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
                addRecommendationToDatabase(foodItem);
            }

            response = "Recommendations stored successfully";
        }
        else if (command == "ROLL_OUT")
        {
            std::cout << "Rolling out Daily food item" << std::endl;

        }

        else if (command == "CREATE_PROFILE")
        {
            std::string userId;
            std::getline(ss, userId, ':');

            std::string profileAnswers;
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

            createEmployeeProfile(userId, foodPreference, spiceLevel, cuisinePreference, sweetTooth);

            response = "Profile updated successfully";
        }
        else if (command == "VIEW_DAILY_MENU")
        {

            std::cout << "Viewing daily menu" << std::endl;
            auto dailyMenuItems = fetchDailyMenuFromDatabase();

            std::string responseData;
            for (const auto &item : dailyMenuItems)
            {
                std::cout << item.foodItemName << std::endl;
                responseData += std::to_string(item.itemId) + ";" + item.foodItemName + ";" + std::to_string(item.price) + ";" + std::to_string(item.rating) + "|";
            }

            if (!responseData.empty())
            {
                responseData.pop_back();
            }

            response = responseData;
        }
        else if (command == "REQUEST_FEEDBACK")
        {
            std::string foodItemName, feedback;
            std::cout << "Feedback REceived ";
            std::getline(ss, foodItemName, ':');
            std::getline(ss, feedback, ':');
            std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
                "INSERT INTO FEEDBACK_RESPONSES (food_item_name, feedback_text) VALUES (?, ?)"));
            pstmt->setString(1, foodItemName);
            pstmt->setString(2, feedback);
            pstmt->execute();
            std::cout << "Feedback received and stored in the database.\n";
        }
        else if (command == "FETCH_FEEDBACK")
        {
            std::string foodItemName;
            std::getline(stream, foodItemName);
            fetchFeedback(foodItemName);
        }
        else
        {
            response = "Unknown command";
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        std::cout << "Response sent: " << response << std::endl;

        memset(buffer, 0, bufferSize);
    }
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
