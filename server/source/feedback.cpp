#include "feedback.h"

FeedbackDetails::FeedbackDetails(int foodItemId, std::string userId, int rating,std::string cmt)
    : foodItemId(foodItemId), userId(userId), rating(rating), feedbackId(0),comment(cmt) {}

// Getters
int FeedbackDetails::getFeedbackId() const
{
    return feedbackId;
}
int FeedbackDetails::getFoodItemId() const
{
    return foodItemId;
}
std::string FeedbackDetails::getUserId() const
{
    return userId;
}
int FeedbackDetails::getRating() const
{
    return rating;
}
std::string FeedbackDetails::getComment() const
{
    return comment;
}

void FeedbackDetails::setFeedbackId(int id)
{
    feedbackId = id;
}
void FeedbackDetails::setFoodItemId(int id)
{
    foodItemId = id;
}
void FeedbackDetails::setUserId(int id)
{
    userId = id;
}
void FeedbackDetails::setRating(int rating)
{
    this->rating = rating;
}
void FeedbackDetails::setComment(const std::string &comment)
{
    this->comment = comment;
}
