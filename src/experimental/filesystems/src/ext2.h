#ifndef __EXT2_H__
#define __EXT2_H__

#include <stdint.h>

struct superblock_t
{
  uint32_t s_inodes_count;
  uint32_t s_blocks_count;
  uint32_t s_r_blocks_count;
  uint32_t s_free_blocks_count;
  uint32_t s_free_inodes_count;
  uint32_t s_first_data_block;
  uint32_t s_log_block_size;
  uint32_t s_log_frag_size;
  
  uint32_t s_blocks_per_group;
  uint32_t s_frags_per_group;
  uint32_t s_inodes_per_group;
  
  uint32_t s_mtime;
  uint32_t s_wtime;
  
  uint16_t s_mnt_count;
  uint16_t s_max_mnt_count;
  
  uint16_t s_magic;
  uint16_t s_state;
  uint16_t s_errors;
  uint16_t s_minor_rev_level;
  
  uint32_t s_lastcheck;
  uint32_t s_checkinterval;
  uint32_t s_creator_os;
  uint32_t s_rev_level;
  
  uint16_t s_def_resuid;
  uint16_t s_def_resgid;
  
  // EXT2_DYNAMIC_REV specific
  uint32_t s_first_ino;
  uint16_t s_inode_size;
  uint16_t s_block_group_nr;
  uint32_t s_feature_compat;
  uint32_t s_feature_incompat;
  uint32_t s_feature_ro_compat;
  
  uint8_t s_uuid[16];
  uint8_t s_volume_name[16];
  uint8_t s_last_mounted[64];
  
  uint32_t s_algo_bitmap;
  
  // Performance hints
  uint8_t s_prealloc_blocks;
  uint8_t s_prealloc_dir_blocks;
  uint16_t padding1;
  
  // Journalling support
  uint8_t s_journal_uuid[16];
  uint32_t s_journal_inum;
  uint32_t s_journal_dev;
  uint32_t s_last_orphan;
  
  // Directory indexing support
  uint32_t s_hash_seed[4];
  uint8_t s_def_hash_version;
  uint8_t padding2[3];
  
  // Other options
  uint32_t s_default_mount_options;
  uint32_t s_first_meta_bg;
  
  uint8_t padding3[760];
} __attribute__((__packed__));

struct blockgroup_t {
  uint32_t bg_block_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  
  uint16_t bg_free_blocks_count;
  uint16_t bg_free_inodes_count;
  uint16_t bg_used_dirs_count;
  uint16_t bg_pad;
  
  uint8_t reserved[12];
} __attribute__((__packed__));

#define EXT2_S_IFDIR 0x4000

struct inode_t {
  uint16_t i_mode;
  uint16_t i_uid;
  uint32_t i_size;
  uint32_t i_atime;
  uint32_t i_ctime;
  uint32_t i_mtime;
  uint32_t i_dtime;
  uint16_t i_gid;
  uint16_t i_links_count;
  uint32_t i_blocks;
  uint32_t i_flags;
  uint32_t i_osd1;
  uint32_t i_block[15];
  uint32_t i_generation;
  uint32_t i_file_acl;
  uint32_t i_dir_acl;
  uint32_t i_faddr;
  uint8_t i_osd2[12];
  
} __attribute__((__packed__));

struct dirent_t {
  uint32_t inode;
  uint16_t rec_len;
  uint8_t name_len;
  uint8_t file_type;
  uint8_t name[255];
} __attribute__((__packed__));

#endif