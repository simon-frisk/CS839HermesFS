#pragma once
#include<string>

#define MAX_FILE_NAME_LENGTH 10

#define PAGE_SIZE 4096 // 4KB Pages

// Enum for the two possible types of an inode - file or folder
enum INodeType {
  folder, file
};

// Struct for an inode
struct INode {
  INodeType type;
  int dataRegionOffset;
  int size;
};

// Store a list of these in the data region for a directory
struct DirectoryData {
  char name[MAX_FILE_NAME_LENGTH + 1];
  int inumber;
};