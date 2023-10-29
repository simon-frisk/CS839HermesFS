#include "hermesfs.h"
#include "defs.h"
#include "util.h"

HermesFS::HermesFS(int capacity, int maxFiles) {
  
  // Allocate data region
  _dataBuffer = new unsigned char[capacity];
  // Allocate inode table
  _inodeTable = new INode[maxFiles];

  // Bitmaps for free space management
  // inode bitmap
  int _inodeTableSize = maxFiles / 8 + (maxFiles % 8 == 0 ? 0 : 1);
  _inodeBitmap = new unsigned char[_inodeTableSize];
  memset(_inodeBitmap, 0, _inodeTableSize);
  // data region bitmap for what pages are free. Note here the page size is 1 byte which is probably not what we want
  int _dataRegionBitMapSize = capacity / 8;
  int _capacity = capacity;
  _dataBitmap = new unsigned char[_dataRegionBitMapSize];
  memset(_dataBitmap, 0, _dataRegionBitMapSize);

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
  int dataRegionLocation = 0; // TODO

  // Populate inode table
  _inodeTable[inumber].type = folder;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = sizeof(DirectoryData);
  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] |= (1 << inumber % 8);

  // TODO: THIS SHOULD BE DONE WHEN CREATE FILE NOT FOLDER
  // Split the path
  std::string name = "test";
  // Make sure the name is not longer than what is allowed
  if (name.length() > MAX_FILE_NAME_LENGTH)
      return;

  // Put the dictionary in the data region
  DirectoryData data;
  memcpy(data.name, name.c_str(), name.length() + 1); // Copy dictionary name into the dictionary data item
  data.inumber = 1;
  memcpy(_dataBuffer, &data, sizeof(DirectoryData));

  // Register data region space as occupied
  for(int i = dataRegionLocation; i < dataRegionLocation + sizeof(DirectoryData); i++)
    _dataBitmap[i / 8] |= (1 << i % 8);
}

void HermesFS::createFile(std::string path, unsigned char* data, int dataLength) {
  
  // Allocate a new INode and set it's contents
  int inumber = allocateINode();    
  if(inumber == -1) return;

  // Find where to put the file in the data region
  int dataRegionLocation = -1;
  int rangeStart = 0;
  // Loop through the data region bitmap and find the first free data region of size large enough
  for (int i = 0; i < _dataRegionBitMapSize * 8; i++) {
      bool isBitSet = (_dataBitmap[i / 8] & (1 << i % 8)) != 0;
      if (isBitSet)
          rangeStart = i + 1;
      else {
          if (i - rangeStart + 1 >= dataLength) {
              dataRegionLocation = rangeStart;
              break;
          }
      }
  }
  // Make sure that a location was found
  if (dataRegionLocation == -1)
      return;

  // Set INode contents
  _inodeTable[inumber].type = file;
  _inodeTable[inumber].dataRegionOffset = dataRegionLocation;
  _inodeTable[inumber].size = dataLength;
  // Register this space as occupied in the inode table bitmap
  _inodeBitmap[inumber / 8] |= (1 << inumber % 8);

  // Copy the file data to the data region
  memcpy(_dataBuffer + dataRegionLocation, data, dataLength);
  // Register this space as occupied in the data region bitmap
  for (int i = dataRegionLocation; i < dataRegionLocation + dataLength; i++)
      _dataBitmap[i / 8] |= (1 << i % 8);
}

void HermesFS::readFile(std::string path, unsigned char* data, int* dataLength) {
    std::vector<std::string> paths = splitPath(path);

    // Look for the "test" file in the root folder
    int inumber = getFileINumberInFolder(0, "test");

    if (inumber == -1)
        return;

    INode fileINode = _inodeTable[inumber];
    
    // Read the data
    *dataLength = fileINode.size;
    memcpy(data, _dataBuffer + fileINode.dataRegionOffset, *dataLength);
}

int HermesFS::allocateINode() {
  // Loop through the bitmap and find the first free inumber
  for (int inumber = 0; inumber < _inodeTableSize * 8; inumber++) {
    if ((_inodeBitmap[inumber / 8] & (1 << (inumber % 8))) == 0) {
        return inumber;
    }
  }
  return -1;
}

int HermesFS::getFileINumberInFolder(int folderINumber, std::string fileName)
{
    INode folderINode = _inodeTable[folderINumber];

    // Iterate through the files/directories in this directory and find the file with this name
    for (int offset = 0; offset < folderINode.size; offset += sizeof(DirectoryData)) {
        DirectoryData* dirData = (DirectoryData*) (_dataBuffer + folderINode.dataRegionOffset + offset);
        if (strcmp(dirData->name, fileName.c_str()) == 0)
            return dirData->inumber;
    }
    // Did not find a matching file
    return -1;
}
