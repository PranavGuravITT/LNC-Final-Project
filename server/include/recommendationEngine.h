#ifndef RECOMMENDATIONENGINE_H
#define RECOMMENDATIONENGINE_H

#include <vector>
#include "feedback.h"
#include "foodItem.h"
#include "database.h"
class RecommendationEngine {
    Database *database;
public:
    RecommendationEngine(Database *database);
    std::vector<FoodItem> getRecommendations(const std::vector<FeedbackDetails> &feedbacks);
    FoodItem fetchFoodItemById(int foodItemId);
};

#endif // RECOMMENDATIONENGINE_H
