#ifndef __SIMULATED_DEVICE_H__
#define __SIMULATED_DEVICE_H__

#include <string>
#include <stdint.h>

#include "raw_storage_provider.h"

class simulated_device : public raw_storage_provider
{
  private:
  FILE *m_handle;
  int m_block_size;
  
  public:
  simulated_device(std::string filename, int block_size = 512);
  ~simulated_device();
  
  virtual void read_block(int block, uint8_t buffer[]);
  virtual int block_size();
  
};

#endif
