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
// std::string Employee::viewDailyMenuRequest(){
//     std::string message = "VIEW_DAILY_MENU";
//     return message;
// }


// void Employee::giveFeedback() {
//     // Implement feedback giving logic here
//     std::cout << "Giving feedback as employee: " << employeeName << std::endl;
//     // Additional logic as needed
// }
