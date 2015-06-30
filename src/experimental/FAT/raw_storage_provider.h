#ifndef __RAW_STORAGE_PROVIDER_H__
#define __RAW_STORAGE_PROVIDER_H__

#include <stdint.h>

class raw_storage_provider
{
  public:
  virtual void read_block(int block, uint8_t buffer[]) = 0;
  virtual int block_size() = 0;
};

#endif


