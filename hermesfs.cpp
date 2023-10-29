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

  // Create the root folder
  createDirectory("/");
}

HermesFS::~HermesFS() {
  delete[] _inodeTable;
  delete[] _dataBuffer;
  delete[] _inodeBitmap;
  delete[] _dataBitmap;
}

void HermesFS::createDirectory(std::string path) {
  // Allocate a new INode and set it's contents
  int inumber = allocateINode();    
  if(inumber == -1) return;

  // Find where to put the folder in the data region
  int dataRegionLocation = 0;

  // Populate inode table
  _inodeTable[inumber].type = folder;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = sizeof(DirectoryData);
  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] &= (1 << inumber % 8);

  // Split the path
  std::string name = "test";

  // Put the folder in the data region
  DirectoryData data;
  data.name = name;
  data.inumber = dataRegionLocation;
  memcpy(_dataBuffer, data, sizeof(DirectoryData));

  // Register data region space as occupied
  for(int i = dataRegionLocation; i < dataRegionLocation + sizeof(DirectoryData); i++) {
    _dataBitmap[i / 8] &= (1 << i % 8);
  }
}

// TODO: Sometimes we might need more than one byte. Currently we just
// allocate one byte in the data region. We also don't set the data region bitmap
void HermesFS::createFile(std::string path, char* data, int dataLength) {
  
  // Allocate a new INode and set it's contents
  int inumber = allocateINode();    
  if(inumber == -1) return;

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

  // Set INode contents
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

void HermesFS::readFile(std::string path, char* data, int* dataLength) {

}

int HermesFS::allocateINode() {
  // Find where to put the file in the itable
  int inumber = 0, inodeBitMapByteSize = strlen(_inodeBitmap); // The second one should be the same as _inodeTableSize

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
    return -1;
  }
}