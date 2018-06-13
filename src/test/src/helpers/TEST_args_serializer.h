#pragma once

#include <helpers/args_serializer/args_serializer.h>
#include <gtest/gtest.h>
#include <string>

// TEST: default input
TEST(ArgsSerializerTest, DefaultInput) {
    int COUNT_ARGS = 1;
    char** argv = new char*[COUNT_ARGS];

    argv[0] = new char[7];
    std::string("./test").copy(argv[0], 7);

    EXPECT_TRUE(helper::serialize::map::args(COUNT_ARGS, argv).empty());

    for (size_t i = 0; i < COUNT_ARGS; ++i)
        delete[] argv[i];
    delete[] argv;
}

// TEST: check for empty input
TEST(ArgsSerializerTest, EmptyInput) {
    int COUNT_ARGS = 0;
    char** argv = nullptr;
    EXPECT_TRUE(helper::serialize::map::args(COUNT_ARGS, argv).empty());
}

// TEST: check for saving expected key
TEST(ArgsSerializerTest, SavingExpectedKey) {
    std::srand(std::time(nullptr));

    int COUNT_ARGS = std::rand() % 200 + 100;
    int TEST_IND = std::rand() % (COUNT_ARGS - 1) + 1;

    char** argv = new char*[COUNT_ARGS];
    for (size_t i = 0; i < COUNT_ARGS; ++i)
        if (i == TEST_IND) {
            argv[i] = new char[5];
            std::string("test").copy(argv[i], 5);
        } else {
            size_t length = std::rand() % 15 + 5;
            argv[i] = new char[length];
            for (size_t j = 0; j < length - 1; ++j)
                argv[i][j] = std::rand() % ('z' - 'a');
            argv[i][length - 1] = '\0';
        }

    EXPECT_TRUE(helper::serialize::map::args(COUNT_ARGS, argv).has("test"));

    for (size_t i = 0; i < COUNT_ARGS; ++i)
        delete[] argv[i];
    delete[] argv;
}

// TEST: check for unexisting key
TEST(ArgsSerializerTest, HandleNotExistingKey) {
    std::srand(std::time(nullptr));

    int COUNT_ARGS = std::rand() % 200 + 100;

    char** argv = new char*[COUNT_ARGS];
    for (size_t i = 0; i < COUNT_ARGS; ++i) {
        size_t length = std::rand() % 15 + 5;
        argv[i] = new char[length];
        for (size_t j = 0; j < length - 1; ++j)
            argv[i][j] = std::rand() % ('z' - 'a');
        argv[i][length - 1] = '\0';
    }

    helper::serialize::map::args(COUNT_ARGS, argv).handle("TEST",
    [] (const helper::serialize::values& vals,
        const helper::serialize::ERR_CODE& err) {
        EXPECT_TRUE(err == helper::serialize::ERR_CODE::KEY_NOT_EXISTS);
    });

    for (size_t i = 0; i < COUNT_ARGS; ++i)
        delete[] argv[i];
    delete[] argv;
}

// TEST: saved values associated with key
TEST(ArgsSerializerTest, HandleKeyValue) {
    std::srand(std::time(nullptr));

    int COUNT_ARGS = std::rand() % 200 + 100;
    int TEST_IND = std::rand() % (COUNT_ARGS - 1) + 1;

    char** argv = new char*[COUNT_ARGS];
    for (size_t i = 0; i < COUNT_ARGS; ++i)
        if (i == TEST_IND) {
            argv[i] = new char[8];
            std::string("TEST=10").copy(argv[i], 8);
        } else {
            size_t length = std::rand() % 15 + 5;
            argv[i] = new char[length];
            for (size_t j = 0; j < length - 1; ++j)
                argv[i][j] = std::rand() % ('z' - 'a');
            argv[i][length - 1] = '\0';
        }

    helper::serialize::map::args(COUNT_ARGS, argv).handle("TEST",
    [] (const helper::serialize::values& vals,
        const helper::serialize::ERR_CODE& err) {
        EXPECT_TRUE(err == helper::serialize::ERR_CODE::NONE);
        EXPECT_EQ(vals.size(), 1);
        EXPECT_EQ(*(vals.begin()), "10");
    });

    for (size_t i = 0; i < COUNT_ARGS; ++i)
        delete[] argv[i];
    delete[] argv;
}
