#ifndef FOODITEM_H
#define FOODITEM_H

#include <iostream>
#include <string>

class FoodItem {
private:
    bool availability;
    int foodItemId;
    int rating;
    double price;
    std::string foodItemName;

public:
    FoodItem();
    FoodItem(const std::string& sname, double price);
    
    // Getters
    std::string getFoodItemName() const;
    bool checkAvailability() const;
    int getRating() const;
    double getPrice() const;
    int getFoodItemId() const;
    
    // Setters
    void setAvailability(bool availability);
    void setFoodItemId(int id);
    void setRating(int rating);
    void setPrice(double price);
    void setFoodItemName(const std::string& name);
};

#endif // FOODITEM_H
