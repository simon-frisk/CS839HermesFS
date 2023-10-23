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
  
  // Size of the regions
  _dataRegionSize = capacity;
  _inodeTableSize = maxFiles;
}

HermesFS::~HermesFS() {
  delete[] _inodeTable;
  delete[] _dataBuffer;
  delete[] _inodeBitmap;
  delete[] _dataBitmap;
}

void HermesFS::createDirectory(std::string name, std::vector<std::string> path) {

  // Find where to put the folder in the itable
  int inumber = 0;
  // Loop through the bitmap and find the first place that can fit the directory
  for(int i = 0; i < inodeBitMapByteSize; i++) {
    // find the best inumber
  }

  // Find where to put the folder in the data region
  int dataRegionLocation = 0;

  // Populate inode table
  _inodeTable[inumber].type = INodeType.folder;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = sizeof(DirectoryData);
  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] &= (1 << inumber % 8);

}

void createFile(std::string name, std::vector<std::string> path, char* data, int dataLength) {

}

void readFile(std::vector<std::string> path, char* data, int* dataLength) {

}