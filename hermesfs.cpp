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
  _inodeTableSize = maxFiles / 8 + (maxFiles % 8 == 0 ? 0 : 1);
  _inodeBitmap = new unsigned char[_inodeTableSize];
  memset(_inodeBitmap, 0, _inodeTableSize);
  // data region bitmap for what pages are free. Note here the page size is 1 byte which is probably not what we want
  _dataRegionBitMapSize = capacity / 8;
  _capacity = capacity;
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
    // Split path
    std::vector<std::string> paths = splitPath(path);

    // Allocate a new INode and set it's contents
    int inumber = allocateINode();    
    if(inumber == -1) return;

    // Unless this is the root directory, add this directory to it's parent
    if (paths.size() != 0) {
        // Find the name of the new directory
        std::string dirName = paths.back();
        paths.pop_back();
        // Make sure the name is not longer than what is allowed
        if (dirName.length() > MAX_FILE_NAME_LENGTH)
            return;
        // Find the inumber of the parent directory by traversing the path
        int parentINumber = traversePathGetInumber(paths);
        if (parentINumber == -1)
            return;
        // Add this directory to the parent
        DirectoryData directoryData;
        directoryData.inumber = inumber;
        strcpy(directoryData.name, dirName.c_str());
        putInDirectory(parentINumber, directoryData);
    }

    // Populate inode table
    _inodeTable[inumber].type = folder;
    _inodeTable[inumber].dataRegionOffset = 0;
    _inodeTable[inumber].size = 0;
    // Register this space as occupied in the inode table bitmap
    _inodeBitmap[inumber / 8] |= (1 << inumber % 8);
}

void HermesFS::updateFile(std::string path, unsigned char* newData, int newDataLength) {
    std::vector<std::string> paths = splitPath(path);

    int fileINumber = traversePathGetInumber(paths);
    if (fileINumber == -1)
        return;

    INode fileINode = _inodeTable[fileINumber];
    
    // Check if the file is a regular file (not a directory)
    if (fileINode.type != file) {
        return;
    }

    if (newDataLength != fileINode.size) {
        // If the new data length is different, we need to allocate a new data region
        int newDataRegionLocation = allocateDataRegionSpace(newDataLength);
        if (newDataRegionLocation == -1)
            return;

        // Copy the new data to the data region
        memcpy(_dataBuffer + newDataRegionLocation, newData, newDataLength);

        for (int i = newDataRegionLocation; i < newDataRegionLocation + newDataLength; i++)
            _dataBitmap[i / 8] |= (1 << i % 8);

        // Free the old data region
        freeDataRegionSpace(fileINode.dataRegionOffset, fileINode.size);

        _inodeTable[fileINumber].dataRegionOffset = newDataRegionLocation;
        _inodeTable[fileINumber].size = newDataLength;
    } else {
        // If the new data length is the same as the old data length, we can simply update the existing data
        memcpy(_dataBuffer + fileINode.dataRegionOffset, newData, newDataLength);
    }
}

void HermesFS::freeDataRegionSpace(int offset, int size) {
    for (int i = offset; i < offset + size; i++) {
        _dataBitmap[i / 8] &= ~(1 << i % 8);
    }
}

void HermesFS::createFile(std::string path, unsigned char* data, int dataLength) {
  // Split the path
  std::vector<std::string> paths = splitPath(path);
  if (paths.size() == 0)
      return;
  std::string fileName = paths.back();
  paths.pop_back();

  // Make sure the name is not longer than what is allowed
  if (fileName.length() > MAX_FILE_NAME_LENGTH)
    return;

  // Allocate a new INode and set it's contents
  int inumber = allocateINode();    
  if(inumber == -1) return;

  int dataRegionLocation = allocateDataRegionSpace(dataLength);
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

  // Update the parent folder to have this file as a child
  DirectoryData dirData;
  strcpy(dirData.name, fileName.c_str()); // Copy dictionary name into the dictionary data item
  dirData.inumber = inumber;

  // Find the inumber of the parent directory and add this file to it's list
  int parentDirectoryINumber = traversePathGetInumber(paths);
  if (parentDirectoryINumber == -1)
      return;
  putInDirectory(parentDirectoryINumber, dirData);
}

void HermesFS::readFile(std::string path, unsigned char* data, int* dataLength) {
    std::vector<std::string> paths = splitPath(path);

    // Traverse the path and find the inumber of the file
    int inumber = traversePathGetInumber(paths);
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

    // Iterate through the files/directories in this directory and find the file/directory with this name
    for (int offset = 0; offset < folderINode.size; offset += sizeof(DirectoryData)) {
        DirectoryData* dirData = (DirectoryData*) (_dataBuffer + folderINode.dataRegionOffset + offset);
        if (strcmp(dirData->name, fileName.c_str()) == 0)
            return dirData->inumber;
    }
    // Did not find a matching file/directory
    return -1;
}

int HermesFS::allocateDataRegionSpace(int size)
{
    // If a region of size 0 was requested, put it at offset 0
    if (size == 0)
        return 0;

    // Find where to put the file in the data region
    int dataRegionLocation = -1;
    int rangeStart = 0;
    // Loop through the data region bitmap and find the first free data region of size large enough
    for (int i = 0; i < _dataRegionBitMapSize * 8; i++) {
        bool isBitSet = (_dataBitmap[i / 8] & (1 << i % 8)) != 0;
        if (isBitSet)
            rangeStart = i + 1;
        else {
            if (i - rangeStart + 1 >= size) {
                dataRegionLocation = rangeStart;
                break;
            }
        }
    }
    // Return the location to put data
    // Note that if no location was found, it will return -1
    return dataRegionLocation;
}

// Add an DirectoryData element to a directory.
void HermesFS::putInDirectory(int directoryINumber, DirectoryData directoryDataItem)
{
    INode inode = _inodeTable[directoryINumber];

    // Allocate a new block of space on the data region
    int dataRegionLocation = allocateDataRegionSpace(inode.size + 1);
    // Check enough space was found
    if (dataRegionLocation == -1)
        return;

    // Copy old data to this new region
    memcpy(_dataBuffer + dataRegionLocation, _dataBuffer + inode.dataRegionOffset, inode.size);
    // Put the new DirectoryData element in the new block
    memcpy(_dataBuffer + dataRegionLocation + inode.size, &directoryDataItem, sizeof(DirectoryData));

    // Register data region space as occupied
    for (int i = dataRegionLocation; i < dataRegionLocation + sizeof(DirectoryData) * (inode.size + 1); i++)
        _dataBitmap[i / 8] |= (1 << i % 8);
    // Register data region space as free
    for (int i = inode.dataRegionOffset; i < inode.dataRegionOffset + sizeof(DirectoryData) * inode.size; i++)
        _dataBitmap[i / 8] |= (1 << i % 8);

    // Change data in inode
    _inodeTable[directoryINumber].dataRegionOffset = dataRegionLocation;
    _inodeTable[directoryINumber].size += sizeof(DirectoryData);
}

int HermesFS::traversePathGetInumber(std::vector<std::string> path)
{
    int inumber = 0;
    for (int i = 0; i < path.size(); i++) {
        inumber = getFileINumberInFolder(inumber, path[i]);
        if (inumber == -1)
            return -1; // Did not find a folder along the path
    }
    return inumber;
}

