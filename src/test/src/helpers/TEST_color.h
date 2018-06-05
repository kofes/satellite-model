#pragma once

#include <helpers/color/color.h>
#include <gtest/gtest.h>
#include <string>

// TEST: default constructor
TEST(ColorTest, DefaultConstructor) {
    helper::color c;
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
}

// TEST: constructor with custom parameters
TEST(ColorTest, ConstructorCustomParameters) {
    std::srand(std::time(nullptr));
    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    EXPECT_EQ(c.r, r);
    EXPECT_EQ(c.g, g);
    EXPECT_EQ(c.b, b);
}

// TEST: access by 'array-operator' ([])
TEST(ColorTest, BracketsOperatorAccess) {
    std::srand(std::time(nullptr));
    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    EXPECT_EQ(c[0], r);
    EXPECT_EQ(c[1], g);
    EXPECT_EQ(c[2], b);
    r += std::rand() % 256;
    g += std::rand() % 256;
    b += std::rand() % 256;
    c[0] = r;
    c[1] = g;
    c[2] = b;
    EXPECT_EQ(c[0], r);
    EXPECT_EQ(c[1], g);
    EXPECT_EQ(c[2], b);
}

// TEST: access to tuple (double)
TEST(ColorTest, TupleDoubleAccess) {
    std::srand(std::time(nullptr));

    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    double T_r, T_g, T_b;
    std::tie(T_r, T_g, T_b) = c.d();
    EXPECT_EQ(T_r, r / 255.);
    EXPECT_EQ(T_g, g / 255.);
    EXPECT_EQ(T_b, b / 255.);
}

// TEST: access to tuple (float)
TEST(ColorTest, TupleFloatAccess) {
    std::srand(std::time(nullptr));

    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    float T_r, T_g, T_b;
    std::tie(T_r, T_g, T_b) = c.f();
    EXPECT_EQ(T_r, r / 255.f);
    EXPECT_EQ(T_g, g / 255.f);
    EXPECT_EQ(T_b, b / 255.f);
}

// TEST: access to elements (double)
TEST(ColorTest, DoubleIndexAccess) {
    std::srand(std::time(nullptr));

    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    EXPECT_EQ(c.d(0), r / 255.);
    EXPECT_EQ(c.d(1), g / 255.);
    EXPECT_EQ(c.d(2), b / 255.);
}

// TEST: access to elements (float)
TEST(ColorTest, FloatIndexAccess) {
    std::srand(std::time(nullptr));

    uint8_t r = std::rand() % 256,
            g = std::rand() % 256,
            b = std::rand() % 256;
    helper::color c(r, g, b);
    EXPECT_EQ(c.f(0), r / 255.f);
    EXPECT_EQ(c.f(1), g / 255.f);
    EXPECT_EQ(c.f(2), b / 255.f);
}
