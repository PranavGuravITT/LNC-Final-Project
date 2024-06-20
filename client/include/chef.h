#ifndef CHEF_H
#define CHEF_H

#include <string>

class Chef {
private:
    std::string chefId;
    std::string password;
    std::string chefName;

public:
    Chef(const std::string& id, const std::string& name, const std::string& password);

    std::string rollOutMenuForNextDay();
    std::string viewMenuRequest();
    void viewResponseFromEmployees();

    std::string getChefId() const;
    std::string getPassword() const;
    std::string getChefName() const;

    void setChefId(const std::string& id);
    void setPassword(const std::string& pass);
    void setChefName(const std::string& name);
};

#endif
