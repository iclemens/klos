
#include "mbr-private.h"
#include "mbr.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

bool mbr::device_qualifies(block_device *device)
{    
  assert(device);  
  
  bool result = false;
  int block_size = device->block_size();
  assert(sizeof(mbr_t) <= block_size);  
  
  uint8_t *buffer = (uint8_t *) malloc(block_size);
  
  try {
    device->read_block(0, buffer);
    
    mbr_t *mbr = (mbr_t *) buffer;
    
    if(mbr->signature == 0xAA55) result = true;
    
  } catch(...) {
  }
  
  free(buffer);
  
  return result;
}
