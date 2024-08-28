#include "admin.h"
#include <iostream>
#include <string>

Admin::Admin(const std::string &id, const std::string &name, const std::string &pass)
    : adminId(id), adminName(name), password(pass) {}


std::string Admin::addFoodItemToMenuRequest()
{
    double price;
    std::string foodItemName;
    std::string message;
    std::string foodType;
    std::string cuisineType;
    std::string spiceLevel;

    std::cout << "Enter Food Item Name: ";
    std::cin >> foodItemName;
    std::cout << "Enter Price: ";
    std::cin >> price;
    std::cout << "Enter Food Type: ";
    std::cin >> foodType;
    std::cout << "Enter Cuisine Type: ";
    std::cin >> cuisineType;
    std::cout << "Enter Spice Level: ";
    std::cin >> spiceLevel;

    message = "ADD_FOOD_ITEM:" + foodItemName + ":" + std::to_string(price) + ":" + foodType + ":" + cuisineType + ":" + spiceLevel;
    return message;
}

std::string Admin::addEmployeeRequest()
{
    std::string id, name, password;
    std::cout << "Enter the ID to be assigned" << std::endl;
    std::cin >> id;
    std::cout << "Enter the name" << std::endl;
    std::cin >> name;
    std::cout << "Enter the password" << std::endl;
    std::cin >> password;
    std::string message = "REGISTER_EMPLOYEE:" + id + ":" + name + ":" + password;
    return message;
}
std::string Admin::deleteEmployeeRequest()
{
    std::string userId;
    std::cout << "Enter User ID to delete: ";
    std::cin >> userId;

    std::string message = "DELETE_USER:" + userId;
    return message;
}
std::string Admin::viewMenuRequest()
{
    std::string message = "VIEW_MENU";
    return message;
}

std::string Admin::deleteFoodItemRequest()
{
    int foodItemId;
    std::cout << "Enter Food Item ID to delete: ";
    std::cin >> foodItemId;

    std::string message = "DELETE_FOOD_ITEM:" + std::to_string(foodItemId);
    return message;
}

std::string Admin::viewDiscardMenuListRequest(){
    std::string message = "VIEW_DISCARD_MENU";
    return message;
}