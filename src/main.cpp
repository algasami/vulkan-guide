#include "vk_engine.h"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "Hello" << std::endl;
  VulkanEngine engine;

  engine.init();

  engine.run();

  engine.cleanup();

  return 0;
}
