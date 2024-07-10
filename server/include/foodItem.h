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
    std::string foodType;
    std::string cuisineType;
    std::string spiceLevel;
    std::string hasSweethTooth;

public:
    FoodItem();
    FoodItem(const std::string& sname, double price);

    // Getters
    std::string getFoodItemName() const;
    bool checkAvailability() const;
    int getRating() const;
    double getPrice() const;
    int getFoodItemId() const;
    std::string getFoodType() const;
    std::string getCuisineType() const;
    std::string getSpiceLevel() const;
    std::string getHasSweethTooth() const;

    // Setters
    void setAvailability(bool availability);
    void setFoodItemId(int id);
    void setRating(int rating);
    void setPrice(double price);
    void setFoodItemName(const std::string& name);
    void setFoodType(const std::string& type);
    void setCuisineType(const std::string& type);
    void setSpiceLevel(const std::string& level);
    void setHasSweethTooth(const std::string& hasSweet);
};

#endif // FOODITEM_H
