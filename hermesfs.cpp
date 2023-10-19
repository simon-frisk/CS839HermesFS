#include "hermesfs.h"

HermesFS::HermesFS(int capacity) {
  _buffer = new char[capacity];
}

HermesFS::~HermesFS() {
  delete[] _buffer;
}