#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>

class Admin{
    private:
    int adminId;
    std::string eadminName;
    std::string password;

    public:
    bool login();
    void addItemToMenu();
};
#endif