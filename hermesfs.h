#pragma once

class HermesFS {
  public:
    HermesFS(int capacity);
    ~HermesFS();
  private:
    char* _buffer;
}