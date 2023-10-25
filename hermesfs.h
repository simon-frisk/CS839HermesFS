#pragma once

#include "defs.h"
#include<string>

class HermesFS {
  public:
    HermesFS(int capacity, int maxFiles);
    ~HermesFS();

    // File system API
    void createDirectory(std::string path);
    void deleteDirectory(std::vector<std::string> path);

    void readFile(std::string path, char* data, int* dataLength);
    void createFile(std::string path, char* data, int dataLength);
    void updateFile(std::string path, char* data, int dataLength);
    void deleteFile(std::string path);

  private:
    INode* _inodeTable;
    char* _dataBuffer;
    // Bitmaps to track free space
    char* _inodeBitmap;
    char* _dataBitmap;
    // Number of bytes allocated for each region
    int _inodeTableSize;
    int _dataRegionSize;
    INodeType _iNodeType;
};