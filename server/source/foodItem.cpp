#include "foodItem.h"

// Default constructor
FoodItem::FoodItem()
    : availability(false), foodItemId(0), rating(0), price(0.0),
      foodItemName(""), foodType(""), cuisineType(""),
      spiceLevel(""), hasSweethTooth("") {}


FoodItem::FoodItem(const std::string& sname, double price)
    : foodItemName(sname), price(price), availability(true),
      foodItemId(0), rating(0), foodType(""), cuisineType(""),
      spiceLevel(""), hasSweethTooth("") {}

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

std::string FoodItem::getFoodType() const {
    return foodType;
}

std::string FoodItem::getCuisineType() const {
    return cuisineType;
}

std::string FoodItem::getSpiceLevel() const {
    return spiceLevel;
}

std::string FoodItem::getHasSweethTooth() const {
    return hasSweethTooth;
}

// Setter for availability
void FoodItem::setAvailability(bool availability) {
    this->availability = availability;
}

// Setter for foodItemId
void FoodItem::setFoodItemId(int id) {
    this->foodItemId = id;
}

// Setter for rating
void FoodItem::setRating(int rating) {
    this->rating = rating;
}

// Setter for price
void FoodItem::setPrice(double price) {
    this->price = price;
}

void FoodItem::setFoodItemName(const std::string& name) {
    this->foodItemName = name;
}

void FoodItem::setFoodType(const std::string& type) {
    this->foodType = type;
}

void FoodItem::setCuisineType(const std::string& type) {
    this->cuisineType = type;
}

// Setter for spiceLevel
void FoodItem::setSpiceLevel(const std::string& level) {
    this->spiceLevel = level;
}

// Setter for hasSweethTooth
void FoodItem::setHasSweethTooth(const std::string& hasSweet) {
    this->hasSweethTooth = hasSweet;
}
