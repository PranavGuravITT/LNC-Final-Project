#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
private:
    std::string message;

public:
    Notification(const std::string& msg) : message(msg) {}

    std::string getMessage() const {
        return message;
    }
};

#endif // NOTIFICATION_H
