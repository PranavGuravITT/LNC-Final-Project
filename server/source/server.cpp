#include "server.h"
#include "feedback.h"

std::vector<FeedbackDetails> Server::fetchFeedbacksFromDatabase()
{
    std::vector<FeedbackDetails> feedbacks;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT feedback_id, food_item_id, employee_id, rating, comment FROM FEEDBACK"));

        while (res->next())
        {
            FeedbackDetails feedback(
                res->getInt("food_item_id"),
                res->getString("employee_id"),
                res->getInt("rating"),
                res->getString("comment"));
            feedback.setFeedbackId(res->getInt("feedback_id"));
            feedbacks.push_back(feedback);
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return feedbacks;
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

bool Server::validateUser(const std::string &userId, const std::string &password)
{

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            userDatabase->getConnection()->prepareStatement("SELECT password FROM EMPLOYEES WHERE employee_id = ?"));

        pstmt->setString(1, userId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            std::string storedPassword = res->getString("password");
            std::cout << storedPassword << std::endl;
            return storedPassword == password;
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}
bool Server::addEmployeeToDatabase(Employee employee)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("INSERT INTO EMPLOYEES (employee_id, employee_name, password) VALUES (?, ?, ?)"));
        pstmt->setString(1, employee.getEmployeeId());
        pstmt->setString(2, employee.getEmployeeName());
        pstmt->setString(3, employee.getPassword());
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}
void Server::clearRecommendationTable()
{
    std::cout << "Clear Recommendation table" << std::endl;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        stmt->execute("DELETE FROM RECOMMENDATIONS");
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
}

void Server::clearDailyMenuTable()
{
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        stmt->execute("DELETE FROM DAILY_MENU");
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
}

bool Server::addRecommendationToDatabase(const FoodItem &item)
{
    try
    {
        std::cout << "Adding data to data base" << std::endl;
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO RECOMMENDATIONS (food_item_id, food_item_name,price,rating) VALUES (?, ?, ?, ?)"));

        pstmt->setInt(1, item.getFoodItemId());
        pstmt->setString(2, item.getFoodItemName());
        pstmt->setDouble(3, item.getPrice());
        pstmt->setInt(4, item.getRating());

        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
        return false;
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

std::string Server::generateDiscardMenuList()
{
    std::ostringstream responseStream;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT F.food_item_id, F.food_item_name, AVG(B.rating) AS avg_rating "
            "FROM FOOD_ITEMS F "
            "JOIN FEEDBACK B ON F.food_item_id = B.food_item_id "
            "GROUP BY F.food_item_id "
            "HAVING avg_rating < 2"));

        while (res->next())
        {
            std::string foodItemId = res->getString("food_item_id");
            std::string foodItemName = res->getString("food_item_name");
            double avgRating = res->getDouble("avg_rating");

            responseStream << foodItemId << ":" << foodItemName << ":" << avgRating << "|";
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return responseStream.str();
}

void Server::handleDiscardMenuOptions(const std::string &command, const std::string &data) {
    if (command == "REMOVE_ITEM") {
        std::string foodItemName;
        std::cout << "Enter Food Item Name to remove: ";
        std::getline(std::cin, foodItemName);

        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "DELETE FROM FOOD_ITEMS WHERE food_item_name = ?"));
        pstmt->setString(1, foodItemName);
        pstmt->execute();

        std::cout << "Food item removed from menu.\n";
    } else if (command == "REQUEST_FEEDBACK") {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO FEEDBACK_RESPONSES (food_item_name, feedback_text) VALUES (?, ?)"));
        pstmt->setString(1, data);
        pstmt->setString(2, data);
        pstmt->execute();
        std::cout << "Feedback received and stored in the database.\n";
    }
}
std::vector<DailyMenu> Server::fetchDailyMenuFromDatabase()
{
    std::vector<DailyMenu> dailyMenu;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT dm.menu_date, dm.item_id, r.food_item_name, r.price,r.rating "
            "FROM DAILY_MENU dm "
            "JOIN RECOMMENDATIONS r ON dm.item_id = r.food_item_id"));

        while (res->next())
        {
            DailyMenu menu;
            menu.menuDate = res->getString("menu_date");
            menu.itemId = res->getInt("item_id");
            menu.foodItemName = res->getString("food_item_name");
            menu.price = res->getDouble("price");
            menu.rating = res->getInt("rating");
            dailyMenu.push_back(menu);
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }

    return dailyMenu;
}

std::vector<FoodItem> Server::fetchFoodItemsFromDatabase()
{
    std::vector<FoodItem> foodItems;
    try
    {
        std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM FOOD_ITEMS"));
        while (res->next())
        {
            FoodItem item;
            item.setFoodItemId(res->getInt("food_item_id"));
            item.setFoodItemName(res->getString("food_item_name"));
            item.setPrice(res->getDouble("price"));
            item.setAvailability(res->getBoolean("availability"));
            foodItems.push_back(item);
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return foodItems;
}
bool Server::createEmployeeProfile(const std::string &employeeId, const std::string &foodPreference,
                                   const std::string &spiceLevel, const std::string &cuisinePreference,
                                   const std::string &hasSweetTooth)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO EMPLOYEE_PROFILES (employee_id, food_preference, spice_level, cuisine_preference, sweet_tooth) "
            "VALUES (?, ?, ?, ?, ?)"));

        pstmt->setString(1, employeeId);
        pstmt->setString(2, foodPreference);
        pstmt->setString(3, spiceLevel);
        pstmt->setString(4, cuisinePreference);
        pstmt->setString(5, hasSweetTooth);

        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}

bool Server::deleteFoodItemFromDatabase(int foodItemId)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("DELETE FROM FOOD_ITEMS WHERE food_item_id = ?"));
        pstmt->setInt(1, foodItemId);
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}
bool Server::incrementVoteCount(int foodItemId)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> checkStmt(userDatabase->getConnection()->prepareStatement(
            "SELECT count_of_votes FROM VOTES WHERE food_item_id = ?"));
        checkStmt->setInt(1, foodItemId);
        std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

        if (res->next())
        {
            int currentCount = res->getInt("count_of_votes");
            std::unique_ptr<sql::PreparedStatement> updateStmt(userDatabase->getConnection()->prepareStatement(
                "UPDATE VOTES SET count_of_votes = ? WHERE food_item_id = ?"));
            updateStmt->setInt(1, currentCount + 1);
            updateStmt->setInt(2, foodItemId);
            updateStmt->execute();
        }
        else
        {
            std::unique_ptr<sql::PreparedStatement> insertStmt(userDatabase->getConnection()->prepareStatement(
                "INSERT INTO VOTES (food_item_id, count_of_votes) VALUES (?, 1)"));
            insertStmt->setInt(1, foodItemId);
            insertStmt->execute();
        }
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}

bool Server::deleteUserFromDatabase(const std::string &userId)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("DELETE FROM EMPLOYEES WHERE employee_id = ?"));
        pstmt->setString(1, userId);
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
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
void Server::fetchFeedback(const std::string &foodItemName) {
    std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
        "SELECT feedback_text FROM FEEDBACK_RESPONSES WHERE food_item_name = ?"));
    pstmt->setString(1, foodItemName);
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    std::string feedbackList;
    while (res->next()) {
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
            int foodItemId;
            ss >> foodItemId;
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

            try
            {
                std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT food_item_id FROM RECOMMENDATIONS"));

                clearDailyMenuTable();

                while (res->next())
                {
                    int foodItemId = res->getInt("food_item_id");

                    try
                    {
                        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
                            "INSERT INTO DAILY_MENU (item_id) VALUES (?)"));

                        pstmt->setInt(1, foodItemId);
                        pstmt->execute();
                    }
                    catch (sql::SQLException &e)
                    {
                        std::cerr << "MySQL error: " << e.what() << std::endl;
                    }
                }

                response = "success";
            }
            catch (sql::SQLException &e)
            {
                std::cerr << "MySQL error: " << e.what() << std::endl;
                response = "failure";
            }
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
            std::string foodItemName,feedback;
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
        else if (command == "FETCH_FEEDBACK") {
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

bool Server::addFeedbackToDatabase(const FeedbackDetails &feedback)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO FEEDBACK (employee_id, food_item_id, rating,comment) VALUES (?, ?, ?, ?)"));
        pstmt->setString(1, feedback.getUserId());
        pstmt->setInt(2, feedback.getFoodItemId());
        pstmt->setInt(3, feedback.getRating());
        pstmt->setString(4, feedback.getComment());
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}
bool Server::addFoodItemToDatabase(const FoodItem &foodItem)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO FOOD_ITEMS (food_item_name, price, availability, food_type, cuisine_type, spice_level) VALUES (?, ?, ?, ?, ?, ?)"));

        pstmt->setString(1, foodItem.getFoodItemName());
        pstmt->setDouble(2, foodItem.getPrice());
        pstmt->setBoolean(3, foodItem.checkAvailability());
        pstmt->setString(4, foodItem.getFoodType());
        pstmt->setString(5, foodItem.getCuisineType());
        pstmt->setString(6, foodItem.getSpiceLevel());
        pstmt->execute();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
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
