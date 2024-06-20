#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>

class Employee{
    private:
    std::string employeeId;
    std::string employeeName;
    std::string password;

    public:
    Employee(std::string empId, const std::string& empName, const std::string& pwd);
    std::string getEmployeeId();
    std::string getEmployeeName();
    std::string getPassword();
    void viewNotifications();
    void giveFeedback();
};
#endif