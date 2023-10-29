#include <iostream>
#include "hermesfs.h"
#include "util.h"

int main() {
  std::cout << "Hello world" << std::endl;

  std::string path = "/hello/my/name/wow";

  std::vector<std::string> paths = splitPath(path);

  HermesFS fs(100, 10);

  unsigned char d = 'a';
  unsigned char* b = &d;
  fs.createFile("/test.txt", b, 1);
  unsigned char* res = new unsigned char[10];
  int len;
  fs.readFile("/test.txt", res, &len);

  int a = 19;
}