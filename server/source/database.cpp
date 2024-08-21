#include "database.h"
#include "foodItem.h"
Database::Database(const std::string &host, const std::string &user, const std::string &password, const std::string &database)
{
    driver = sql::mysql::get_mysql_driver_instance();
    std::cout << "database active" << std::endl;
    connection = driver->connect("tcp://" + host + ":3306", user, password);
    connection->setSchema(database);
}

Database::~Database()
{
    delete connection;
}

sql::Connection *Database::getConnection()
{
    return connection;
}

bool Database::validateUser(const std::string &userId, const std::string &password)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(getConnection()->prepareStatement("SELECT password FROM EMPLOYEES WHERE employee_id = ?"));

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

bool Database::addFoodItemToDatabase(const FoodItem &foodItem)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(getConnection()->prepareStatement(
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

std::vector<FoodItem> Database::fetchFoodItemsFromDatabase()
{
    std::vector<FoodItem> foodItems;
    try
    {
        std::unique_ptr<sql::Statement> stmt(getConnection()->createStatement());
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

std::string Database::fetchNotificationsFromDatabase()
{
    std::string notifications;
    try
    {
        std::unique_ptr<sql::Statement> stmt(getConnection()->createStatement());
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

bool Database::deleteFoodItemFromDatabase(int foodItemId)
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

bool Database::deleteUserFromDatabase(const std::string &userId)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(getConnection()->prepareStatement("DELETE FROM EMPLOYEES WHERE employee_id = ?"));
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

bool Database::addFeedbackToDatabase(const FeedbackDetails &feedback)
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


std::vector<FeedbackDetails> Database::fetchFeedbacksFromDatabase()
{
    std::vector<FeedbackDetails> feedbacks;
    try
    {
        std::unique_ptr<sql::Statement> stmt(getConnection()->createStatement());
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

std::string Database::getDiscardMenuList()
{
    std::ostringstream responseStream;
    try
    {
        std::unique_ptr<sql::Statement> stmt(getConnection()->createStatement());
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

bool Database::incrementVoteCountInDatabase(int foodItemId)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> checkStmt(getConnection()->prepareStatement(
            "SELECT count_of_votes FROM VOTES WHERE food_item_id = ?"));
        checkStmt->setInt(1, foodItemId);
        std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

        if (res->next())
        {
            int currentCount = res->getInt("count_of_votes");
            std::unique_ptr<sql::PreparedStatement> updateStmt(getConnection()->prepareStatement(
                "UPDATE VOTES SET count_of_votes = ? WHERE food_item_id = ?"));
            updateStmt->setInt(1, currentCount + 1);
            updateStmt->setInt(2, foodItemId);
            updateStmt->execute();
        }
        else
        {
            std::unique_ptr<sql::PreparedStatement> insertStmt(getConnection()->prepareStatement(
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

void Database::clearRecommendationTable()
{
    try
    {
        std::unique_ptr<sql::Statement> stmt(getConnection()->createStatement());
        stmt->execute("DELETE FROM RECOMMENDATIONS");
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

bool Database::storeEmployeeProfileInDatabase(const std::string &employeeId, const std::string &foodPreference,
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

std::vector<DailyMenu> Database::fetchDailyMenuFromDatabase()
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

std::string Database::fetchFeedbackForAnFoodItem(const std::string &foodItemName)
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

    return feedbackList;
    send(clientSocket, feedbackList.c_str(), feedbackList.length(), 0);
}
void Database::clearDailyMenuTable()
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
bool Database::addFoodItemsToDailyMenu(){

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