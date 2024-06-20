#include "employee.h"

// Constructor
Employee::Employee(std::string empId, const std::string& empName, const std::string& pwd)
    : employeeId(empId), employeeName(empName), password(pwd) {
    // Initialize any other necessary data members
}

// Getters
std::string Employee::getEmployeeId() {
    return employeeId;
}

std::string Employee::getEmployeeName() {
    return employeeName;
}

std::string Employee::getPassword() {
    return password;
}

// Member functions
bool Employee::login() {
    // Implement login logic here
    // For simplicity, let's say login is successful if employeeId and password match
    // int inputId;
    // std::string inputPwd;

    // std::cout << "Enter employee ID: ";
    // std::cin >> inputId;
    // std::cout << "Enter password: ";
    // std::cin >> inputPwd;

    // return (inputId == employeeId && inputPwd == password);
    return true;
}

void Employee::viewNotifications() {
    // Implement order placing logic here
    std::cout << "Placing order for employee: " << employeeName << std::endl;
    // Additional logic as needed
}

void Employee::giveFeedback() {
    // Implement feedback giving logic here
    std::cout << "Giving feedback as employee: " << employeeName << std::endl;
    // Additional logic as needed
}
