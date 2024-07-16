#include "chef.h"
#include <iostream>
#include <string>


Chef::Chef(const std::string& id, const std::string& name, const std::string& password)
    : chefId(id), chefName(name), password(password) {}



std::string Chef::rollOutMenuForNextDay() {
 std::cout<<"Rolling out Menu"<<std::endl;
 std::string message = "ROLL_OUT";
 return message;
}

std::string Chef::getReccommendationRequest(){
  std::string message = "GET_RECOMMENDATIONS";
  return message;
}

std::string Chef::checkNotificationsRequest() {
    std::string message = "CHECK_NOTIFICATIONS";
    return message;
}

std::string Chef::viewMenuRequest() {
    std::string message = "VIEW_MENU";
    return message;
}

void Chef::viewResponseFromEmployees(){

}


std::string Chef::getChefId() const {
    return chefId;
}

std::string Chef::getPassword() const {
    return password;
}

std::string Chef::getChefName() const {
    return chefName;
}


void Chef::setChefId(const std::string& id) {
    chefId = id;
}

void Chef::setPassword(const std::string& pass) {
    password = pass;
}

void Chef::setChefName(const std::string& name) {
    chefName = name;
}
