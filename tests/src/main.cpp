// Tests assume that ofxApp/tests is the working directory

#include <stdio.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

GTEST_API_ int main(int argc, char **argv) {
    printf("Running ofxApp test suite\n");
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
