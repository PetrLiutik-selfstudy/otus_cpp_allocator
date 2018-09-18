#include "gtest/gtest.h"
#include "../inc/custom_allocator.h"
#include "../inc/custom_vector.h"
#include "../inc/factorial.h"
#include "../inc/ver.h"

TEST(ver_test_case, ver_major_test) {
  EXPECT_GE(ver_major(), 1);
}

TEST(ver_test_case, ver_minor_test) {
  EXPECT_GE(ver_minor(), 1);
}

TEST(ver_test_case, ver_patch_test) {
  EXPECT_GE(ver_patch(), 1);
}

TEST(factorial_test, factorial) {
  EXPECT_EQ(factorial(0), 1);
  EXPECT_EQ(factorial(1), 1);
  EXPECT_EQ(factorial(2), 2);
  EXPECT_EQ(factorial(3), 6);
  EXPECT_EQ(factorial(4), 24);
  EXPECT_EQ(factorial(5), 120);
  EXPECT_EQ(factorial(6), 720);
  EXPECT_EQ(factorial(7), 5040);
  EXPECT_EQ(factorial(8), 40320);
  EXPECT_EQ(factorial(9), 362880);
  EXPECT_EQ(factorial(10), 3628800);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
