
#include "server.h"

Server::Server(int port, Database *database) : port_(port), userDatabase(database), server_fd_(0), client_socket_(0), addrlen_(sizeof(address_))
{
    memset(&address_, 0, sizeof(address_));
    create_socket();
    bind_socket();
    initialize_user_db();
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

try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            userDatabase->getConnection()->prepareStatement("SELECT password FROM users WHERE user_id = ?"));

        pstmt->setString(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            std::string db_password = res->getString("password");
            return db_password == password;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;

}

void Server::initialize_user_db()
{
    // Initializing a simple user database

    std::unique_ptr<sql::Statement> stmt(userDatabase->getConnection()->createStatement());
    stmt->execute("CREATE TABLE IF NOT EXISTS EMPLOYEES ("
                  "employee_id INT AUTO_INCREMENT PRIMARY KEY, "
                  "emplyee_name VARCHAR(255) NOT NULL UNIQUE, "
                  "password VARCHAR(255) NOT NULL)");

    stmt->execute("CREATE TABLE IF NOT EXISTS food_items ("
                      "food_item_id INT PRIMARY KEY, "
                      "food_item_name VARCHAR(100) NOT NULL, "
                      "price DOUBLE NOT NULL, "
                      "availability BOOLEAN NOT NULL, "
                      "rating INT DEFAULT 0)");

    stmt->execute("INSERT IGNORE INTO users (user_id, password) VALUES "
                  "('user1', 'password1'), "
                  "('user2', 'password2'), "
                  "('user3', 'password3')");
    stmt->execute("INSERT IGNORE INTO food_items (food_item_name, price,availability,rating) VALUES "
                  "('dosa', '80','true','0')");
    // user_db_["user1"] = "password1";
    // user_db_["user2"] = "password2";
    // user_db_["user3"] = "password3";
}

void Server::handle_client() {
    char buffer[buffer_size_] = {0};
    while (true) {
        read(client_socket_, buffer, buffer_size_);
        std::string request(buffer);

        std::istringstream ss(request);
        std::string command;
        std::getline(ss, command, ':');

        std::cout << "Command received " << command << std::endl;

        std::string response;

        if (command == "VALIDATE") {
            std::cout << "Validate" << std::endl;
            std::string user_id, password;
            std::getline(ss, user_id, ':');
            std::getline(ss, password, ':');

            if (validate_user(user_id, password)) {
                response = "Authentication successful";
            } else {
                response = "Authentication failed";
            }
        }
        else if (command == "REGISTER_EMPLOYEE") {
            std::cout << "Registering employee" << std::endl;
            std::string employeeName;
            int employeeId;
            std::string password;
            ss >> employeeId;
            ss.ignore();
            std::getline(ss, employeeName, ':');
            ss >> password;
             foodItem(food_name, food_id, price);
            if (addEmployeeToDatabase(foodItem)) {
                response = "Employee added successfully";
            } else {
                response = "Failed to add new employee";
            }
        }
        else if (command == "ADD_FOOD_ITEM") {
            std::cout << "Adding food item" << std::endl;
            std::string food_name;
            int food_id;
            double price;
            ss >> food_id;
            ss.ignore();
            std::getline(ss, food_name, ':');
            ss >> price;
            FoodItem foodItem(food_name, food_id, price);
            if (addFoodItemToDatabase(foodItem)) {
                response = "Food item added successfully";
            } else {
                response = "Failed to add food item";
            }
        }
        else {
            response = "Unknown command";
        }

        send(client_socket_, response.c_str(), response.length(), 0);
        std::cout << "Response sent: " << response << std::endl;

        // Clear buffer for next read
        memset(buffer, 0, buffer_size_);
    }
}


bool Server::addFoodItemToDatabase(const FoodItem &foodItem) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(userDatabase->getConnection()->prepareStatement("INSERT INTO food_items (food_item_id, food_item_name, price, availability, rating) VALUES (?, ?, ?, ?, ?)"));
        pstmt->setInt(1, foodItem.getFoodItemId());
        pstmt->setString(2, foodItem.getFoodItemName());
        pstmt->setDouble(3, foodItem.getPrice());
        pstmt->setBoolean(4, foodItem.checkAvailability());
        pstmt->setInt(5, foodItem.getRating());
        pstmt->execute();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return false;
}
void Server::run()
{
    listen_for_connections();
    accept_connection();
    handle_client();
}
