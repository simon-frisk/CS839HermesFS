#pragma once

#include "defs.h"

class HermesFS {
  public:
    HermesFS(int capacity, int maxFiles);
    ~HermesFS();

    // File system API
    void createDirectory(std::string name, std::vector<std::string> path);
    

  private:
    char* _inodeTable;
    char* _dataBuffer;
}