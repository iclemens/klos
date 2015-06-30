#ifndef __BLOCK_DEVICE_MANAGER_H__
#define __BLOCK_DEVICE_MANAGER_H__

#include <string>
#include <map>
#include "block_device.h"

typedef dq_ptr bool mbr::(device_qualifies *)(block_device *device)

class block_device_manager
{
  private:
  std::map<std::string, block_device *> m_devices;
  
  public:
  bool register_device(std::string name, block_device *device);
};

#endif
