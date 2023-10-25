#include "hermesfs.h"
#include "defs.h"

HermesFS::HermesFS(int capacity, int maxFiles) {
  
  // Allocate data region
  _dataBuffer = new char[capacity];
  // Allocate inode table
  _inodeTable = new INode[maxFiles];

  // Bitmaps for free space management
  // inode bitmap
  _inodeBitmap = new char[maxFiles / 8 + (maxFiles % 8 == 0 ? 0 : 1)];
  // data region bitmap for what pages are free. Note here the page size is 1 byte which is probably not what we want
  _dataBitmap = new char[capacity / 8 + (maxFiles % 8 == 0 ? 0 : 1)];
  
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

void HermesFS::createDirectory(std::string path) {

  // Find where to put the folder in the itable
  int inumber = 0, inodeBitMapByteSize = strlen(_inodeBitmap);
  // Loop through the bitmap and find the first place that can fit the directory
  for(int i = 0; i < inodeBitMapByteSize; i++) {
    // find the best inumber
  }

  // Find where to put the folder in the data region
  int dataRegionLocation = 0;

  // Populate inode table
  _iNodeType = folder;
  _inodeTable[inumber].type = _iNodeType;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = sizeof(DirectoryData);
  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] &= (1 << inumber % 8);

}

void HermesFS::createFile(std::string path, char* data, int dataLength) {
  // Find where to put the file in the itable
  int inumber = 0, inodeBitMapByteSize = strlen(_inodeBitmap);

  // Loop through the bitmap and find the first place that can fit the file
  for (int i = 0; i < inodeBitMapByteSize; i++) {
    // Find the first free inode
    if (!(_inodeBitmap[i] & (1 << inumber % 8))) {
      break;
    }
    inumber++;
  }

  // Check if we found a free inode
  if (inumber >= _inodeTableSize) {
    return;
  }

  // Find where to put the file in the data region
  int dataRegionLocation = 0, dataRegionBitMapBytesSize = strlen(_inodeBitmap);
  // Loop through the data region bitmap and find the first free data region
  for (int i = 0; i < dataRegionBitMapBytesSize; i++) {
    if (!(_dataBitmap[i] == 0xFF)) {
      // Find the first free byte within this data region
      for (int j = 0; j < 8; j++) {
        if (!(_dataBitmap[i] & (1 << j))) {
          dataRegionLocation = i * 8 + j;
          _dataBitmap[i] |= (1 << j);
          break;
        }
      }
      if (dataRegionLocation > 0) {
        break;
      }
    }
  }

  _iNodeType = file;
  _inodeTable[inumber].type = _iNodeType;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = dataLength;

  // Copy the file data to the data region
  if (dataRegionLocation * _dataRegionSize + dataLength <= _dataRegionSize) {
    memcpy(_dataBuffer + dataRegionLocation * _dataRegionSize, data, dataLength);
  } 

  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] |= (1 << inumber % 8);
}

void readFile(std::vector<std::string> path, char* data, int* dataLength) {

}