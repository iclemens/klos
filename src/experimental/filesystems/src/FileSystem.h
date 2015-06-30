#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

class EInvalidSuperblock { };
class EMemoryAllocationFailed { };

class File
{  
  public:
  virtual void Seek(uint32_t index) = 0;
  virtual void Read(uint32_t count, void *buffer) = 0;
  virtual uint32_t GetSize() = 0;
};

class Directory
{
};

class Filesystem
{
  public:
  virtual File *openFile(uint32_t id) = 0;
  virtual Directory *openDirectory(uint32_t id) = 0;
  
  
  virtual void createFile(const char *name);
  virtual void createDirectory(const char *name);
};

#endif
