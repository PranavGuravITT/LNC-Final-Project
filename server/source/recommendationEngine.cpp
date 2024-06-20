#include "recommendationEngine.h"
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "database.h"
RecommendationEngine::RecommendationEngine(Database *database):database(database){}
std::vector<FoodItem> RecommendationEngine::getRecommendations(const std::vector<FeedbackDetails> &feedbacks) {
    std::unordered_map<int, int> ratingSums;  
    std::unordered_map<int, int> ratingCounts;  

    for (const auto &feedback : feedbacks) {
        
            ratingSums[feedback.getFoodItemId()] += feedback.getRating();
            ratingCounts[feedback.getFoodItemId()] += 1;
        
    }

    std::vector<std::pair<int, double>> avgRatings;  

    for (const auto &entry : ratingSums) {
        int foodItemId = entry.first;
        double avgRating = static_cast<double>(entry.second) / ratingCounts[foodItemId];
        avgRatings.emplace_back(foodItemId, avgRating);
    }

    std::sort(avgRatings.begin(), avgRatings.end(), [](const auto &firstElement, const auto &secondElement) {
        return firstElement.second > secondElement.second;
    });

    std::vector<FoodItem> recommendations;
    for (const auto &iterator : avgRatings) {
        FoodItem item = fetchFoodItemById(iterator.first);
        item.setRating(iterator.second);
        recommendations.push_back(item);
    }

    return recommendations;
}

FoodItem RecommendationEngine::fetchFoodItemById(int foodItemId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(database->getConnection()->prepareStatement(
            "SELECT food_item_name, price, availability, rating FROM FOOD_ITEMS WHERE food_item_id = ?"));
        pstmt->setInt(1, foodItemId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            FoodItem item(
                res->getString("food_item_name"),
                res->getDouble("price")
            );
            item.setFoodItemId(foodItemId);
            item.setAvailability(res->getBoolean("availability"));
            item.setRating(res->getInt("rating"));
            return item;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    }
    return FoodItem("", 0.0);  
}
