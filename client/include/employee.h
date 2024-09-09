#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>

class Employee{
    private:
    std::string employeeId;
    std::string employeeName;
    std::string password;
    void inputProfileDetails(std::string &dietaryPreference, std::string &spiceLevel, std::string &cuisineType, std::string &hasSweetTooth);

    public:
    Employee() = default;
    Employee(std::string empId, const std::string& empName, const std::string& pwd);
    std::string getEmployeeId();
    std::string getEmployeeName();
    std::string getPassword();
    std::string viewDailyMenuRequest();
    std::string feedbackRequest();
    std::string voteforFoodItemRequest();
    std::string Employee::createProfileRequest()
};
#endif