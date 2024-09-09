#include "employee.h"
#include <gtest/gtest.h>

class EmployeeTest : public ::testing::Test {
protected:
    void SetUp() override {
        employee = new Employee("EMP001", "John Doe", "password123");
    }

    void TearDown() override {
        delete employee;
    }

    Employee* employee;
};

TEST_F(EmployeeTest, GetEmployeeIdTest) {
    EXPECT_EQ(employee->getEmployeeId(), "EMP001");
}

TEST_F(EmployeeTest, GetEmployeeNameTest) {
    EXPECT_EQ(employee->getEmployeeName(), "John Doe");
}

TEST_F(EmployeeTest, GetPasswordTest) {
    EXPECT_EQ(employee->getPassword(), "password123");
}

