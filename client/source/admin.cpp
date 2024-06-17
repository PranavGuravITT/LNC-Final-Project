#include "Admin.hpp"
#include <iostream>
#include <string>

// Constructor
Admin::Admin(int id, const std::string& name, const std::string& pass)
    : adminId(id), adminName(name), password(pass) {}

// Simulated login function
bool Admin::login(const std::string& name, const std::string& pass) const {
    return (adminName == name && password == pass);
}

// Function to add a food item to the menu
void Admin::addFoodItemToMenu() {
    std::stringstream ss;
    ss << "ADD:1:Pizza:9.99";
}
