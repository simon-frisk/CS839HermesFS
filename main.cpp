#include <iostream>
#include "hermesfs.h"
#include "util.h"

int main() {
  HermesFS fs(100, 10);

  unsigned char d = 'a';
  unsigned char* b = &d;
  fs.createDirectory("/dir");
  fs.createFile("/dir/test.txt", b, 1);
  unsigned char* res = new unsigned char[10];
  int len;
  fs.readFile("/dir/test.txt", res, &len);

  int a = 19;
}