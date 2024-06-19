#ifndef FEEDBACKDETAILS_H
#define FEEDBACKDETAILS_H

#include <string>

class FeedbackDetails {
private:
    int feedbackId;
    int foodItemId;
    std::string userId;
    int rating;
    std::string comment;

public:
    FeedbackDetails(int foodItemId, std::string userId, int rating);

    // Getters
    int getFeedbackId() const;
    int getFoodItemId() const;
    std::string getUserId() const;
    int getRating() const;
    std::string getComment() const;

    // Setters
    void setFeedbackId(int id);
    void setFoodItemId(int id);
    void setUserId(int id);
    void setRating(int rating);
    void setComment(const std::string &comment);
};

#endif // FEEDBACKDETAILS_H
