#ifndef __SIMULATEDDEVICE_H_
#define __SIMULATEDDEVICE_H_

#include "BlockDevice.h"

#include <stdio.h>
#include <stdint.h>

class SimulatedDevice :public BlockDevice
{
  private:
  FILE *source;
  uint32_t block_size;
  
  public:
  SimulatedDevice(const char *filename);
  ~SimulatedDevice();
  
  void ReadData(uint32_t offset, uint32_t size, void *buffer);
};

#endif
