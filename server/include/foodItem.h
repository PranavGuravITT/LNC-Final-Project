#ifndef FOODITEM_H
#define FOODITEM_H

#include <iostream>

class FoodItem{
    private:
    bool availability;
    int foodItemId;
    int rating;
    double price;
    std::string foodItemName;

    public:
    FoodItem(const std::string& sname, int id, double price);
    std::string getFoodItemName() const;
    void setAvailability(bool availability) ;
    bool checkAvailability() const;
    int getRating() const;
    double getPrice() const;
    int getFoodItemId() const;
};

#endif