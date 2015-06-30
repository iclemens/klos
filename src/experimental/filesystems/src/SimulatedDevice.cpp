
#include "SimulatedDevice.h"

#include <stdlib.h>

SimulatedDevice::SimulatedDevice(const char *filename)
{
  source = fopen(filename, "r");  
  
  if(source == NULL) {
    printf("Error: could not open source\n");
    exit(1);
  }
  
  printf("device: opened\n");
}

SimulatedDevice::~SimulatedDevice()
{
  fclose(source);
  
  printf("device: closed\n");
}

void SimulatedDevice::ReadData(uint32_t offset, uint32_t size, void *buffer)
{
  fseek(source, offset, SEEK_SET);
  fread(buffer, size, 1, source);
}
