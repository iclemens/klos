#ifndef __FILESYSTEMEXT2_H__
#define __FILESYSTEMEXT2_H__

#include "BlockDevice.h"
#include "FileSystem.h"
#include "ext2.h"

class Extended2File;

class Extended2 :public Filesystem
{
  private:
  BlockDevice *device;

  uint32_t block_size;  
  
  superblock_t superblock;
  blockgroup_t *blockgroups;
  
  uint32_t num_blockgroups;
  
  protected:
  void ReadBlocks(uint32_t block, uint32_t count, void *buffer);
  void ReadInodeInfo(uint32_t id, inode_t *inode);
  
  uint32_t GetBlockSize();
  
  uint32_t allocateInode();  
  uint32_t allocateBlock(uint32_t near_block);
  void deallocateBlock(uint32_t block);
  
  
  public:
  Extended2(BlockDevice *device);
  ~Extended2();
  
  File *createFile(uint32_t id, uint8_t name[]);
  File *openFile(uint32_t id);
  
  Directory *openDirectory(uint32_t id);
  
  friend class Extended2File;
  friend class Extended2Directory;
};

class Extended2File :public File
{
  private:
  Extended2 *filesystem;
  inode_t inode;
  uint32_t index;
  
  public:
  Extended2File(Extended2 *filesystem, uint32_t id);
  
  void Seek(uint32_t index);
  void Read(uint32_t count, void *buffer);
  uint32_t GetSize();
};

class Extended2Directory :private Directory
{
  private:
  Extended2 *filesystem;
  Extended2File *directory;
  
  public:
  Extended2Directory(Extended2 *filesystem, uint32_t id);
  ~Extended2Directory();
    
  void getEntryAtOffset(uint32_t offset, dirent_t *dirent);
};

#endif