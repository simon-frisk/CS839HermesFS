#pragma once

#include "defs.h"

class HermesFS {
  public:
    HermesFS(int capacity, int maxFiles);
    ~HermesFS();

    // File system API
    void createDirectory(std::string name, std::vector<std::string> path);
    void createFile(std::string name, std::vector<std::string> path, char* data, int dataLength);
    void readFile(std::vector<std::string> path, char* data, int* dataLength);
    void deleteFile(std::vector<std::string> path);
    void deleteDirectory(std::vector<std::string> path);

  private:
    INode* _inodeTable;
    char* _dataBuffer;
    // Bitmaps to track free space
    char* _inodeBitmap;
    char* _dataBitmap;
}