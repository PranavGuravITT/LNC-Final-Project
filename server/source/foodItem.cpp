#include "foodItem.h"

FoodItem::FoodItem() = default;
FoodItem::FoodItem(const std::string& sname, double price)
    : foodItemName(sname), price(price), availability(true), rating(0), foodItemId(0) {}

std::string FoodItem::getFoodItemName() const {
    return foodItemName;
}

bool FoodItem::checkAvailability() const {
    return availability;
}

int FoodItem::getRating() const {
    return rating;
}

double FoodItem::getPrice() const {
    return price;
}

int FoodItem::getFoodItemId() const {
    return foodItemId;
}

void FoodItem::setAvailability(bool availability) {
    this->availability = availability;
}

void FoodItem::setFoodItemId(int id) {
    this->foodItemId = id;
}

void FoodItem::setRating(int rating) {
    this->rating = rating;
}

void FoodItem::setPrice(double price) {
    this->price = price;
}

void FoodItem::setFoodItemName(const std::string& name) {
    this->foodItemName = name;
}
