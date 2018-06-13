#include <gtest/gtest.h>
#include "src/helpers/TEST_helpers.h"
#include "src/math_models/TEST_math_models.h"

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
