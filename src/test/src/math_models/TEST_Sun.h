#include <math_models/Sun/Sun.h>
#include <helpers/helpers.h>
#include <gtest/gtest.h>

TEST(SunTest, Mass) {
    math::model::Sun Sun;
    EXPECT_EQ(Sun.mass(), helper::constant::SUN_MASS);
}

TEST(SunTest, Radius) {
    math::model::Sun Sun;
    EXPECT_EQ(Sun.R(), helper::constant::SUN_RADIUS);
    EXPECT_EQ(Sun.scale()[0][0], Sun.R());
    EXPECT_EQ(Sun.scale()[1][1], Sun.R());
    EXPECT_EQ(Sun.scale()[2][2], Sun.R());
}
