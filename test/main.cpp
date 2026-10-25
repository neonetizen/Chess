#include <gtest/gtest.h>

#include "attacks.hpp"

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  chess::init_attacks();
  return RUN_ALL_TESTS();
}
