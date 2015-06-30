#ifndef __BLOCKDEVICE_H_
#define __BLOCKDEVICE_H_

#include <stdint.h>

class BlockDevice
{
  public:
  virtual void ReadData(uint32_t offset, uint32_t size, void *buffer) = 0;
};

#endif