#include "gtest/gtest.h"
#include "../inc/custom_allocator.h"
#include "../inc/custom_vector.h"
#include "../inc/factorial.h"
#include "../inc/ver.h"

#include <array>

TEST(ver_test_case, ver_major_test) {
  EXPECT_GE(ver_major(), 1);
}

TEST(ver_test_case, ver_minor_test) {
  EXPECT_GE(ver_minor(), 1);
}

TEST(ver_test_case, ver_patch_test) {
  EXPECT_GE(ver_patch(), 1);
}

TEST(factorial_test_case, factorial_test) {
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

TEST(allocator_test_case, allocate_one_test)
{
  constexpr size_t N = 1;
  custom::allocator<int, N> allocator;
  int* ptr{nullptr};

  try {
    ptr = allocator.allocate(N);
  }
  catch (const std::bad_alloc &e) {
    FAIL() << e.what();
  }

  EXPECT_NE(ptr, nullptr);
  EXPECT_EQ(sizeof(allocator), sizeof(std::array<int, N>) + sizeof(std::array<bool, N>));
}

TEST(allocator_test_case, allocate_ten_test)
{
  constexpr size_t N = 10;
  custom::allocator<int, N> allocator;
  int* ptr{nullptr};

  try {
    ptr = allocator.allocate(N);
  }
  catch (const std::bad_alloc &e) {
    FAIL() << e.what();
  }

  EXPECT_NE(ptr, nullptr);
  EXPECT_EQ(sizeof(allocator), sizeof(std::array<int, N>) + sizeof(std::array<bool, N>));
}

TEST(allocator_test_case, allocate_fail_test)
{
  constexpr size_t N = 10;
  custom::allocator<int, N> allocator;
  int* ptr{nullptr};
  bool is_bad_alloc{false};

  try {
    ptr = allocator.allocate(N + 1);
  }
  catch (const std::bad_alloc &e) {
    is_bad_alloc = true;
  }

  EXPECT_TRUE(is_bad_alloc);
}

TEST(allocator_test_case, deallocate_test) {
  constexpr size_t N = 10;
  custom::allocator<int, N> allocator;
  int* ptr{nullptr};

  try {
    ptr = allocator.allocate(N);
    allocator.deallocate(ptr, N);
  }
  catch (const std::bad_alloc &e) {
    FAIL() << e.what();
  }
}

TEST(allocator_test_case, allocate_cust_heap_test)
{
  constexpr size_t N = 10;
  custom::allocator<int, 0> allocator;
  int* ptr{nullptr};

  try {
    ptr = allocator.allocate(N);
  }
  catch (const std::bad_alloc &e) {
    FAIL() << e.what();
  }

  EXPECT_NE(ptr, nullptr);
}

TEST(allocator_test_case, deallocate_cust_heap_test) {
  constexpr size_t N = 10;
  custom::allocator<int, 0> allocator;
  int* ptr{nullptr};

  try {
    ptr = allocator.allocate(N);
    allocator.deallocate(ptr, N);
  }
  catch (const std::bad_alloc &e) {
    FAIL() << e.what();
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
