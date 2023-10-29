#include <iostream>
#include "hermesfs.h"
#include "util.h"

int main() {
  std::cout << "Hello world" << std::endl;

  std::string path = "/hello/my/name/wow";

  std::vector<std::string> paths = splitPath(path);

  HermesFS fs(50, 5);

  char* d= 0xc4;
  fs.createFile("/test.txt", d, 1);
}