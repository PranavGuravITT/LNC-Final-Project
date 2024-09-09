#include "employee.h"


Employee::Employee(std::string empId, const std::string& empName, const std::string& pwd)
    : employeeId(empId), employeeName(empName), password(pwd) {
}

std::string Employee::getEmployeeId() {
    return employeeId;
}

std::string Employee::getEmployeeName() {
    return employeeName;
}

std::string Employee::getPassword() {
    return password;
}

std::string Employee::viewDailyMenuRequest(){
    return "VIEW_DAILY_MENU";
}

std::string Employee::feedbackRequest(){
    int foodItemId;
    int rating;
    std::string comment;

    std::cout << "Enter Food Item ID: ";
    std::cin >> foodItemId;
    std::cin.ignore();

    std::cout << "Enter Rating(0-5): ";
    std::cin >> rating;
    std::cin.ignore();

    std::cout << "Enter Comment: ";
    std::getline(std::cin, comment);
    std::string message = "ADD_FEEDBACK:" + getEmployeeId() + ":" + std::to_string(foodItemId) + ":" + std::to_string(rating) + ":" + comment;
    return message;
}

std::string Employee::voteforFoodItemRequest(){
    int foodItemId;
    std::cout << "Enter Food Item ID to vote for: ";
    std::cin >> foodItemId;

    std::string message = "INCREMENT_VOTE:" + std::to_string(foodItemId);
    return message;
}

void Employee::inputProfileDetails(std::string &dietaryPreference, std::string &spiceLevel, std::string &cuisineType, std::string &hasSweetTooth) {
    std::cout << "Enter Dietary Preference (Vegetarian, Non Vegetarian, Eggetarian): ";
    std::getline(std::cin, dietaryPreference);
    while (dietaryPreference != "Vegetarian" && dietaryPreference != "Non Vegetarian" && dietaryPreference != "Eggetarian") {
        std::cout << "Invalid input. Enter Dietary Preference (Vegetarian, Non Vegetarian, Eggetarian): ";
        std::getline(std::cin, dietaryPreference);
    }

    std::cout << "Enter Spice Level (High, Medium, Low): ";
    std::getline(std::cin, spiceLevel);
    while (spiceLevel != "High" && spiceLevel != "Medium" && spiceLevel != "Low") {
        std::cout << "Invalid input. Enter Spice Level (High, Medium, Low): ";
        std::getline(std::cin, spiceLevel);
    }

    std::cout << "Enter Cuisine Type (North Indian, South Indian, Other): ";
    std::getline(std::cin, cuisineType);
    while (cuisineType != "North Indian" && cuisineType != "South Indian" && cuisineType != "Other") {
        std::cout << "Invalid input. Enter Cuisine Type (North Indian, South Indian, Other): ";
        std::getline(std::cin, cuisineType);
    }

    std::cout << "Enter Sweet Tooth (Yes, No): ";
    std::getline(std::cin, hasSweetTooth);
    while (hasSweetTooth != "Yes" && hasSweetTooth != "No") {
        std::cout << "Invalid input. Enter Sweet Tooth (Yes, No): ";
        std::getline(std::cin, hasSweetTooth);
    }
}

std::string Employee::createProfileRequest(){
    std::string employeeId, dietaryPreference, spiceLevel, cuisineType, hasSweetTooth;
    inputProfileDetails(dietaryPreference, spiceLevel, cuisineType, hasSweetTooth);

    std::string message = "CREATE_PROFILE:" + employeeId + ":" + dietaryPreference + ":" + spiceLevel + ":" + cuisineType + ":" + hasSweetTooth;
    return message;
}