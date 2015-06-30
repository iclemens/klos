
#include "FileSystemExt2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Initializes the Extended 2 filesystem interface by reading
 * superblock and group descriptor table.
 */
Extended2::Extended2(BlockDevice *device)
{
  this->device = device;
  
  // Read super block
  device->ReadData(1024, sizeof(superblock_t), &superblock);

  if(superblock.s_magic != 0xEF53)
    throw new EInvalidSuperblock();

  block_size = 1024 << superblock.s_log_block_size;
  
  // Read block group descriptors
  num_blockgroups = superblock.s_blocks_count / superblock.s_blocks_per_group + ((superblock.s_blocks_count % superblock.s_blocks_per_group > 0)?1:0);    
  blockgroups = new blockgroup_t[num_blockgroups];
  
  if(blockgroups == NULL)    
    throw new EMemoryAllocationFailed();
  
  uint32_t blockgroup_size = sizeof(blockgroup_t) * num_blockgroups;
  uint32_t blocks = blockgroup_size / block_size + (blockgroup_size % block_size > 0?1:0);

  uint8_t *buffer = new uint8_t[blocks * block_size];  
  
  if(buffer == NULL) {
    delete[] blockgroups;    
    blockgroups = NULL;
    
    throw new EMemoryAllocationFailed();
  }
     
  ReadBlocks(superblock.s_first_data_block + 1, blocks, (void *) buffer);   
  memcpy(blockgroups, buffer, blockgroup_size);
  delete[] buffer;
     
  printf("ext2: found %d blockgroups contained in %d block(s)\n", num_blockgroups, blocks);  
  printf("ext2: filesystem loaded\n");
}


/**
 * Free memory structures associated with the Extended 2 filesystem
 */
Extended2::~Extended2()
{
  if(blockgroups != NULL)
    delete[] blockgroups;
  
  printf("ext2: filesystem unloaded\n");
}


Directory *Extended2::openDirectory(uint32_t id)
{
  return (Directory *) new Extended2Directory(this, id);
}


/**
 * 
 */
File *Extended2::createFile(uint32_t id, uint8_t name[])
{
  // Check if file already exists
  /*Directory *directory = filesystem->openDirectory(id);  
  delete directory;*/
  
  // 1 - allocate new inode  
  // 2 - store in directory list
  
  return NULL;
}


File *Extended2::openFile(uint32_t id)
{
  return (File *) new Extended2File(this, id);
}




void Extended2::ReadBlocks(uint32_t block, uint32_t count, void *buffer)
{
  device->ReadData(block * block_size, block_size * count, buffer);
}

void Extended2::ReadInodeInfo(uint32_t id, inode_t *inode)
{
  uint32_t group = (id - 1) / superblock.s_inodes_per_group;
  uint32_t index = (id - 1) % superblock.s_inodes_per_group;
  
  if(group >= num_blockgroups) {
    printf("Error: invalid inode %d\n", id);
    exit(1);
  }
  // check for valid group
  
  uint32_t inode_block = blockgroups[group].bg_inode_table;
   
  uint32_t block = (index * superblock.s_inode_size) / block_size;
  uint32_t block_index = (index * superblock.s_inode_size) % block_size;

  uint8_t data[block_size];

  for(int i = 0; i < block_size; i++) {
  //  printf("%x ", data[i]);
  }
  
  ReadBlocks(inode_block + block, 1, data); 
  memcpy(inode, data + block_index, sizeof(inode_t));
}

uint32_t Extended2::GetBlockSize()
{
  return block_size;
}


/**
 * Open extended 2 directory
 */
Extended2Directory::Extended2Directory(Extended2 *filesystem, uint32_t id)
{
  this->filesystem = filesystem;
  directory = (Extended2File *) filesystem->openFile(id);
}


/**
 * Close extended 2 directory
 */
Extended2Directory::~Extended2Directory()
{
  delete directory;
}


/**
 * Open file on extended 2 filesystem
 */
Extended2File::Extended2File(Extended2 *filesystem, uint32_t id)
{
  this->filesystem = filesystem;
  filesystem->ReadInodeInfo(id, &inode);
  
  printf("File opened: %d, size: %d, mode: %x\n", id, inode.i_size, inode.i_mode);  
}

void Extended2File::Seek(uint32_t index)
{
  this->index = index;
}

void Extended2File::Read(uint32_t count, void *buffer)
{
  uint32_t block_size = filesystem->GetBlockSize();
  
  uint32_t blidx = index / block_size;
  uint32_t btidx = index % block_size;  
  
  uint8_t block[block_size];  
  filesystem->ReadBlocks(inode.i_block[blidx], 1, block);
  
  // 1-12 -> Direct
  // 13 -> Single indirect
  // 14 -> Double indirect
  // 15 -> Triple indirect
  
  if(btidx + count > block_size) {
    printf("Error: data cannot be read\n");
    exit(1);
  }
  
  memcpy(buffer, block + btidx, count);
}

uint32_t Extended2File::GetSize()
{
  return inode.i_size;
}

