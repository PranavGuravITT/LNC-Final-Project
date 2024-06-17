#include "foodItem.h"
#include <iostream>

// Constructor implementation
FoodItem::FoodItem(const std::string& sname, int id, double price)
    : foodItemName(sname), foodItemId(id), price(price), availability(true), rating(0) {}

// Method to print the food item name
std::string FoodItem::getFoodItemName() const {
    std::cout << "Food Item Name: " << foodItemName << std::endl;
    return foodItemName;
}

// Method to set the availability of the food item
void FoodItem::setAvailability(bool availability) {
    this->availability = availability;
}

// Method to check the availability of the food item
bool FoodItem::checkAvailability() const {
    return availability;
}
int FoodItem::getRating() const{
    return rating;
}
double FoodItem::getPrice() const{
return price;
}
int FoodItem::getFoodItemId() const{ 
    return foodItemId;
}