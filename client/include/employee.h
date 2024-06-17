#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>

class Employee{
    private:
    int employeeId;
    std::string employeeName;
    std::string password;

    public:
    Employee::Employee(int empId, const std::string& empName, const std::string& pwd);
    int getEmployeeId();
    std::string getEmployeeName();
    std::string getPassword();

    bool login();
    void placeOrder();
    void giveFeedback()
};
#endif