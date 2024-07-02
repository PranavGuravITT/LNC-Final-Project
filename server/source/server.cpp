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
    std::cout << "Clear table" << std::endl;
    std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
        "DELETE FROM RECOMMENDATIONS"));
}

void Server::clearDailyMenuTable()
{
    std::cout << "Clear table" << std::endl;
    std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
        "DELETE FROM DAILY_MENU"));
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
                  "rating INT DEFAULT 0)");

    stmt->execute("CREATE TABLE IF NOT EXISTS FEEDBACK ("
                  "feedback_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "food_item_id INT, "
                  "employee_id VARCHAR(255), "
                  "rating INT, "
                  "comment VARCHAR(255), "
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id) ON DELETE CASCADE, "
                  "FOREIGN KEY (employee_id) REFERENCES EMPLOYEES(employee_id) ON DELETE CASCADE)");

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

// std::string formatDailyMenuAsString(const std::vector<DailyMenu> &dailyMenu) {
//     std::ostringstream oss;

//     for (const auto &menu : dailyMenu) {
//         oss << menu.itemId << "," << menu.foodItemName<< "," << menu.rating << "\n";
//     }

//     return oss.str();
// }
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
            item.setRating(res->getInt("rating"));
            foodItems.push_back(item);
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return foodItems;
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
            std::string food_name;
            std::getline(ss, food_name, ':');
            ss >> price;
            FoodItem foodItem(food_name, price);
            if (addFoodItemToDatabase(foodItem))
            {
                response = "Food item added successfully";
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

            // Remove the last pipe delimiter
            if (!responseData.empty())
            {
                responseData.pop_back();
            }

            response = responseData;
        }
        else if (command == "STORE_RECOMMENDATIONS")
        {
            std::string recommendationsStr;
            std::getline(ss, recommendationsStr);

            std::cout << "Received String: " << recommendationsStr << std::endl; // Debug print

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
            auto feedbacks = fetchFeedbacksFromDatabase();
            RecommendationEngine engine(userDatabase);
            auto recommendationFoodItems = engine.getRecommendations(feedbacks);

            clearDailyMenuTable();
            for (const auto &item : recommendationFoodItems)
            {
                try
                {
                    std::cout << item.getFoodItemId() << std::endl;
                    std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
                        "INSERT INTO DAILY_MENU (item_id) VALUES (?)"));

                    pstmt->setInt(1, item.getFoodItemId());
                    pstmt->execute(); 
                }
                catch (sql::SQLException &e)
                {
                    std::cerr << "MySQL error: " << e.what() << std::endl;
                }
            }

            response = "success";
        }
        else if (command == "VIEW_DAILY_MENU")
        {

            std::cout << "Viewing daily menu" << std::endl;
            auto dailyMenuItems = fetchDailyMenuFromDatabase();

            std::string responseData;
            for (const auto &item : dailyMenuItems)
            {
                responseData += std::to_string(item.itemId) + ";" + item.foodItemName + ";" + std::to_string(item.price) + ";" + std::to_string(item.rating) + "|";
            }

            if (!responseData.empty())
            {
                responseData.pop_back();
            }

            response = responseData;
        }

        else
        {
            response = "Unknown command";
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        std::cout << "Response sent: " << response << std::endl;

        // Clear buffer for next read
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
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("INSERT INTO FOOD_ITEMS (food_item_name, price, availability, rating) VALUES (?, ?, ?, ?)"));

        pstmt->setString(1, foodItem.getFoodItemName());
        pstmt->setDouble(2, foodItem.getPrice());
        pstmt->setBoolean(3, foodItem.checkAvailability());
        pstmt->setInt(4, foodItem.getRating());
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
