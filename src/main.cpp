#include <iostream>

#include "attacks.hpp"
#include "log.hpp"

int main() {
  CHESS_TRACE_FUNC();
  chess::init_attacks();
  std::cout << "hello world" << '\n';
}
