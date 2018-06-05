#include <math_models/Earth/Earth.h>
#include <helpers/helpers.h>
#include <gtest/gtest.h>

TEST(EarthTest, Mass) {
    math::model::Earth E;
    EXPECT_EQ(E.mass(), helper::constant::EARTH_MASS);
}

TEST(EarthTest, Radius) {
    math::model::Earth E;
    EXPECT_EQ(E.R(), helper::constant::EARTH_R);
    EXPECT_EQ(E.eR(), helper::constant::EARTH_E_R);
    EXPECT_EQ(E.pR(), helper::constant::EARTH_P_R);
    EXPECT_EQ(E.scale()[0][0], E.eR());
    EXPECT_EQ(E.scale()[1][1], E.eR());
    EXPECT_EQ(E.scale()[2][2], E.pR());
}
