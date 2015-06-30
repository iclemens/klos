
#include "block_device_manager.h"

bool block_device_manager::register_device(std::string name, block_device *device)
{
  m_devices[name] = device;
  return true;
}
