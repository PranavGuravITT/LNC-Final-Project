#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <sstream>

class Admin
{
private:
    std::string adminId;
    std::string adminName;
    std::string password;

public:
    Admin(const std::string &id, const std::string &name, const std::string &pass);

    std::string addFoodItemToMenuRequest();
    std::string addEmployeeRequest();
    std::string deleteEmployeeRequest();
    std::string viewMenuRequest();
    std::string deleteFoodItemRequest();
    std::string viewDiscardMenuListRequest();
};

#endif
