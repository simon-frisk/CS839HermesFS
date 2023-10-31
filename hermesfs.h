#pragma once

#include "defs.h"
#include <string>
#include <vector>
#include <string>

class HermesFS {
  public:
    HermesFS(int capacity, int maxFiles);
    ~HermesFS();

    // File system API
    void createDirectory(std::string path);
    void deleteDirectory(std::string path);

    void readFile(std::string path, unsigned char* data, int* dataLength);
    void createFile(std::string path, unsigned char* data, int dataLength);
    void updateFile(std::string path, unsigned char* data, int dataLength);
    void deleteFile(std::string path);

  private:

    // Helper methods that are needed several times
    int allocateINode();
    int getFileINumberInFolder(int folderINumber, std::string fileName);
    int allocateDataRegionSpace(int size);
    void putInDirectory(int directoryINumber, DirectoryData directoryDataItem);
    int traversePathGetInumber(std::vector<std::string> path);
    void freeDataRegionSpace(int offset, int size);

    // Inote table and data region
    INode* _inodeTable;
    unsigned char* _dataBuffer;
    // Bitmaps to track free space
    unsigned char* _inodeBitmap;
    unsigned char* _dataBitmap;
    // Number of bytes allocated for each region
    int _inodeTableSize;
    int _dataRegionBitMapSize;
    int _capacity;
    INodeType _iNodeType;
};
