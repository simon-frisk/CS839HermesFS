#include "hermesfs.h"

HermesFS::HermesFS(int capacity, int maxFiles) {
  
  // Allocate data region
  _dataBuffer = new char[capacity];
  // Allocate inode table
  _inodeTable = new INode[maxFiles];

  // Bitmaps for free space management
  // inode bitmap
  int inodeBitMapByteSize = maxFiles / 8 + (maxFiles % 8 == 0 ? 0 : 1);
  _inodeBitmap = new char[bitMapByteSize];
  // data region bitmap for what pages are free. Note here the page size is 1 byte which is probably not what we want
  int dataRegionBitMapBytesSize = capacity / 8 + (maxFiles % 8 == 0 ? 0 : 1);
  _dataBitmap = new char[dataRegionBitMapBytesSize];

}

HermesFS::~HermesFS() {
  delete[] _inodeTable;
  delete[] _dataBuffer;
  delete[] _inodeBitmap;
  delete[] _dataBitmap;
}

void HermesFS::createDirectory(std::string name, std::vector<std::string> path) {

}

void createFile(std::string name, std::vector<std::string> path, char* data, int dataLength) {

}

void readFile(std::vector<std::string> path, char* data, int* dataLength) {
  
}