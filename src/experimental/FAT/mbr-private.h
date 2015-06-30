#ifndef __MBR_PRIVATE_H__
#define __MBR_PRIVATE_H__

#include <stdint.h>

struct mbr_partition_t
{
  uint8_t status; 
  uint8_t chs_first[3];
  uint8_t partition_type;
  uint8_t chs_last[3];
  uint32_t lba_first;
  uint32_t number_of_sectors;
} __attribute__ (( __packed__ ));

struct mbr_t
{
  char boot_code[440];
  
  uint32_t disk_signature;
  uint16_t reserved;
  
  struct mbr_partition_t paritions[4];
  
  uint16_t signature;
} __attribute__ (( __packed__ ));

#endif
