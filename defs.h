#pragma once

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
  std::string name;
  int inumber;
}