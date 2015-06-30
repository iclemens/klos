
#include "simulated_device.h"

#include <stdio.h>
#include <stdint.h>

simulated_device::simulated_device(std::string filename, int block_size)
{
  m_block_size = block_size;
  m_handle = fopen(filename.c_str(), "r");
}

void simulated_device::read_block(int block, uint8_t buffer[])
{
  fseek(m_handle, block * m_block_size, SEEK_SET);
  fread(buffer, 1, m_block_size, m_handle);
}

int simulated_device::block_size()
{
  return m_block_size;
}

simulated_device::~simulated_device()
{
  if(m_handle)
    fclose(m_handle);
}
