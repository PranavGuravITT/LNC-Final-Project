#include "foodItem.h"
#include <gtest/gtest.h>

class FoodItemTest : public ::testing::Test {
protected:
    void SetUp() override {
        foodItem = new FoodItem("Pizza", 12.99);
    }

    void TearDown() override {
        delete foodItem;
    }

    FoodItem* foodItem;
};

TEST_F(FoodItemTest, DefaultConstructor_InitializesWithDefaultValues) {
    FoodItem defaultFoodItem;
    EXPECT_EQ(defaultFoodItem.getFoodItemName(), "");
    EXPECT_EQ(defaultFoodItem.getPrice(), 0.0);
    EXPECT_FALSE(defaultFoodItem.checkAvailability());
    EXPECT_EQ(defaultFoodItem.getFoodItemId(), 0);
    EXPECT_EQ(defaultFoodItem.getRating(), 0);
    EXPECT_EQ(defaultFoodItem.getFoodType(), "");
    EXPECT_EQ(defaultFoodItem.getCuisineType(), "");
    EXPECT_EQ(defaultFoodItem.getSpiceLevel(), "");
}

TEST_F(FoodItemTest, ParameterizedConstructor_InitializesCorrectly) {
    EXPECT_EQ(foodItem->getFoodItemName(), "Pizza");
    EXPECT_EQ(foodItem->getPrice(), 12.99);
    EXPECT_TRUE(foodItem->checkAvailability());
}

TEST_F(FoodItemTest, SetAndGetFoodItemName) {
    foodItem->setFoodItemName("Burger");
    EXPECT_EQ(foodItem->getFoodItemName(), "Burger");
}

TEST_F(FoodItemTest, SetAndGetAvailability) {
    foodItem->setAvailability(false);
    EXPECT_FALSE(foodItem->checkAvailability());
}

TEST_F(FoodItemTest, SetAndGetFoodItemId) {
    foodItem->setFoodItemId(101);
    EXPECT_EQ(foodItem->getFoodItemId(), 101);
}

TEST_F(FoodItemTest, SetAndGetRating) {
    foodItem->setRating(5);
    EXPECT_EQ(foodItem->getRating(), 5);
}

TEST_F(FoodItemTest, SetAndGetPrice) {
    foodItem->setPrice(15.49);
    EXPECT_EQ(foodItem->getPrice(), 15.49);
}

TEST_F(FoodItemTest, SetAndGetFoodType) {
    foodItem->setFoodType("Fast Food");
    EXPECT_EQ(foodItem->getFoodType(), "Fast Food");
}

TEST_F(FoodItemTest, SetAndGetCuisineType) {
    foodItem->setCuisineType("American");
    EXPECT_EQ(foodItem->getCuisineType(), "American");
}

TEST_F(FoodItemTest, SetAndGetSpiceLevel) {
    foodItem->setSpiceLevel("Medium");
    EXPECT_EQ(foodItem->getSpiceLevel(), "Medium");
}
