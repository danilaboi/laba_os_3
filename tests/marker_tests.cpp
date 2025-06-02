#include "gtest/gtest.h"
#include "marker.h"
#include "shared.h"

class MarkerLogicTest : public ::testing::Test {
protected:
    SharedData shared;

    void SetUp() override {
        shared.size = 10;
        shared.array = new int[shared.size] {};
        InitializeCriticalSection(&shared.arrayLock);
    }

    void TearDown() override {
        DeleteCriticalSection(&shared.arrayLock);
        delete[] shared.array;
    }
};

TEST_F(MarkerLogicTest, TryMarkIndex_SucceedsWhenZero) {
    int index = 3;
    bool result = TryMarkIndex(&shared, 1, index);
    EXPECT_TRUE(result);
    EXPECT_EQ(shared.array[index], 1);
}

TEST_F(MarkerLogicTest, TryMarkIndex_FailsWhenNonZero) {
    shared.array[5] = 2;
    bool result = TryMarkIndex(&shared, 1, 5);
    EXPECT_FALSE(result);
    EXPECT_EQ(shared.array[5], 2);
}

TEST_F(MarkerLogicTest, UnmarkIndices_ClearsOnlySpecified) {
    shared.array[2] = 1;
    shared.array[4] = 1;
    std::vector<int> indices = { 2, 4 };
    UnmarkIndices(&shared, indices);
    EXPECT_EQ(shared.array[2], 0);
    EXPECT_EQ(shared.array[4], 0);
}
