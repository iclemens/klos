#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>

struct fat_bootsector_h 
{
    char jump[3];
    
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t fat_count;
    uint16_t max_root_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint32_t hidden_sector_count;
    uint32_t total_sectors;
    
    union extended {
      struct fat16_bootsector_h;
      struct fat32_bootsector_h;
    };
    
    
    uint16_t signature;
} __attributes__((__packed__));

struct fat16_bootsector_h
{
    uint8_t drive_number;
    uint8_t dirty;
    uint8_t extended_boot_signature;
    uint32_t serial_number;
    char volume_label[11];
    char fat_type[8];
    
    char boot_code[448];
} __attributes__((__packed__));

struct fat32_bootsector_h
{
  uint32_t sectors_per_fat;
  uint16_t fat_flags;
  uint16_t version;
  uint32_t first_root_cluster;
  uint16_t fs_info_sector;
  uint16_t boot_sector_copy;
  uint8_t reserved1[12];
  uint8_t drive_number[1];
  uint8_t reserved2;
  uint8_t extended_boot_signature;
  uint32_t serial_number;
  char volume_label[11];
  char fat_type[8];
    
  char boot_code[420];
} __attributes__((__packed__));

#endif