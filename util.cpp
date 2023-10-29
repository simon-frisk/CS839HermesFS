#include "util.h"

std::vector<std::string> splitPath(std::string path) {
  std::vector<std::string> paths;

  std::string temp;
  int lastSep = 0;
  int i = 1;

  // Has to start at root folder
  if (!path[0] == '/')
      return paths;
  // Iterate through the path and split when encountering '/'
  while(path[i] != '\0') {
    // Check if separator. If so, split. Else, append to temp
    if(path[i] == '/') {
      paths.push_back(temp);
      temp = "";
    } else {
      temp += path[i];
    }

    // Increment counter
    i++;
  }

  return paths;
}