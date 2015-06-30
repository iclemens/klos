#ifndef __MBR_H__
#define __MBR_H__

#include "mbr-private.h"
#include "block_device.h"

class mbr_factory
{
  public:
  bool device_qualifies(block_device *device);
  block_device_container *create_object(block_device *parent);  
};

class mbr
{
  protected:
  mbr_t master_boot_record;
  
  public:
  mbr();
  ~mbr();   
};

#endif