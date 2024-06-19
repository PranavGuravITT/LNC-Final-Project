#ifndef CHEF_HPP
#define CHEF_HPP

#include <iostream>

class Chef{
    private:
    std::string chefId;
    std::string password;
    std::string chefName;
    public:
    bool login();
    public:
    void rollOutMenu();
};
#endif 
