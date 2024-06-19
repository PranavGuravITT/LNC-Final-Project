
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
                res->getInt("rating"));
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

Server::Server(int port, Database *database) : port_(port), userDatabase(database), server_fd_(0), client_socket_(0), addrlen_(sizeof(address_))
{
    memset(&address_, 0, sizeof(address_));
    create_socket();
    bind_socket();
    initializeDatabase();
}

Server::~Server()
{
    close(client_socket_);
    close(server_fd_);
}

void Server::create_socket()
{
    if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void Server::bind_socket()
{
    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::listen_for_connections()
{
    if (listen(server_fd_, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::accept_connection()
{
    if ((client_socket_ = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t *)&addrlen_)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
}

bool Server::validate_user(const std::string &user_id, const std::string &password)
{

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            userDatabase->getConnection()->prepareStatement("SELECT password FROM EMPLOYEES WHERE employee_id = ?"));

        pstmt->setString(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            std::string db_password = res->getString("password");
            std::cout << db_password << std::endl;
            return db_password == password;
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
bool Server::addRecommendationToDatabase(const FoodItem &item) {
    try {
        std::cout<<"Adding data to data base"<<std::endl;
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO RECOMMENDATIONS (food_item_id, food_item_name, price, availability, rating) VALUES (?, ?, ?, ?, ?)"));

        pstmt->setInt(1, item.getFoodItemId());
        pstmt->setString(2, item.getFoodItemName());
        pstmt->setDouble(3, item.getPrice());
        pstmt->setBoolean(4, item.checkAvailability());
        pstmt->setInt(5, item.getRating());

        pstmt->execute();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
        return false;
    }
}

void Server::initializeDatabase()
{
    // Initializing a simple user database

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
                  "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id), "
                  "FOREIGN KEY (employee_id) REFERENCES EMPLOYEES(employee_id))");

    stmt->execute("CREATE TABLE IF NOT EXISTS RECOMMENDATIONS ("
              "id INT AUTO_INCREMENT PRIMARY KEY, "
              "food_item_id INT NOT NULL, "
              "food_item_name VARCHAR(255) NOT NULL, "
              "price DOUBLE NOT NULL, "
              "availability BOOLEAN NOT NULL, "
              "rating INT NOT NULL, "
              "FOREIGN KEY (food_item_id) REFERENCES FOOD_ITEMS(food_item_id))");
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
bool Server::deleteUserFromDatabase(const std::string &user_id)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("DELETE FROM EMPLOYEES WHERE employee_id = ?"));
        pstmt->setString(1, user_id);
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
    char buffer[buffer_size_] = {0};
    while (true)
    {
        read(client_socket_, buffer, buffer_size_);
        std::string request(buffer);
        std::cout<<"Request "<<request<<std::endl;
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
            if (validate_user(userId, password))
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
            Json::Value jsonData;
            for (const auto &item : foodItems)
            {
                Json::Value jsonItem;
                jsonItem["food_item_id"] = item.getFoodItemId();
                jsonItem["food_item_name"] = item.getFoodItemName();
                jsonItem["price"] = item.getPrice();
                jsonItem["availability"] = item.checkAvailability();
                jsonItem["rating"] = item.getRating();
                jsonData.append(jsonItem);
            }
            Json::StreamWriterBuilder writer;
            response = Json::writeString(writer, jsonData);
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
            std::string user_id;
            std::getline(ss, user_id, ':');
            if (deleteUserFromDatabase(user_id))
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
            std::string employeeId;
            int foodItemId, rating;
            std::getline(ss, employeeId, ':');
            ss >> foodItemId;
            ss.ignore(1, ':');
            ss >> rating;

            std::cout<<"Adding feedback"<<std::endl;
            std::cout << employeeId << "  " << rating << "  " << foodItemId << std::endl;
            FeedbackDetails feedback(foodItemId, employeeId, rating);
            std::cout<<"Adding feedback2"<<std::endl;
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
        responseData += std::to_string(item.getFoodItemId()) + ";"
                     + item.getFoodItemName() + ";"
                     + std::to_string(item.getPrice()) + ";"
                     + (item.checkAvailability() ? "1" : "0") + ";"
                     + std::to_string(item.getRating()) + "|";
    }
    
    // Remove the last pipe delimiter
    if (!responseData.empty()) {
        responseData.pop_back();
    }

    response = responseData;
}
else if (command == "STORE_RECOMMENDATIONS") {
    std::string recommendationsStr;
    std::getline(ss, recommendationsStr);

    std::cout << "Received String: " << recommendationsStr << std::endl;  // Debug print

    std::istringstream recommendationsStream(recommendationsStr);
    std::string recommendation;
    while (std::getline(recommendationsStream, recommendation, '|')) {
        std::istringstream recommendationStream(recommendation);
        std::string token;

        std::vector<std::string> fields;
        while (std::getline(recommendationStream, token, ';')) {
            fields.push_back(token);
        }
        
    std::cout<<"hello"<<std::endl;
    std::cout<<token<<std::endl;
            FoodItem foodItem(fields[1], std::stod(fields[2]));
            foodItem.setFoodItemId(std::stoi(fields[0]));
            foodItem.setAvailability(fields[3] == "1");
            foodItem.setRating(std::stoi(fields[4]));
            addRecommendationToDatabase(foodItem);
        
    }

    response = "Recommendations stored successfully";
}


        else
        {
            response = "Unknown command";
        }

        send(client_socket_, response.c_str(), response.length(), 0);
        std::cout << "Response sent: " << response << std::endl;

        // Clear buffer for next read
        memset(buffer, 0, buffer_size_);
    }
}

bool Server::addFeedbackToDatabase(const FeedbackDetails &feedback)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement(
            "INSERT INTO FEEDBACK (employee_id, food_item_id, rating) VALUES (?, ?, ?)"));
        std::cout<<"Hello "<<feedback.getRating()<<std::endl;
        pstmt->setString(1, feedback.getUserId());
        pstmt->setInt(2, feedback.getFoodItemId());
        pstmt->setInt(3, feedback.getRating());
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
    listen_for_connections();
    accept_connection();
    handleClient();
}
