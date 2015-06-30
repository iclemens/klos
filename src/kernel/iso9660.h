#include "config.h"

#ifndef __ISO9660_H_
#define __ISO9660_H_

// 2048 byte blocks, is always present in the header blocks
// type = 0 means empty (boot info)
// type = 1 is the PVD
// type = 255 is the termination block

struct iso9660_primary_volume_descriptor {
	uint8_t	type;
	uint8_t id;
	uint8_t version;
	uint8_t _1;
	uint8_t system_id;
	uint8_t volume_id;
	uint8_t _2;
	uint8_t volume_space_size;
	uint8_t _3;
	uint8_t volume_set_size;
	uint8_t volume_sequence_number;
	uint8_t logical_block_size;
	uint8_t path_table_size;
	uint8_t type_l_path_table;
	uint8_t opt_type_l_path_table;
	uint8_t type_m_path_table;
	uint8_t opt_type_m_path_table;
	uint8_t root_directory_record;
	uint8_t volume_set_id;
	uint8_t publisher_id;
	uint8_t preparer_id;
	uint8_t application_id;
	uint8_t copyright_file_id;
	uint8_t abstract_file_id;
	uint8_t bibliographic_file_id;
	uint8_t creation_date;
	uint8_t modification_date;
	uint8_t expiration_date;
	uint8_t effective_date;
	uint8_t file_structure_version;
	uint8_t _4;
	uint8_t application_data;
	uint8_t _5;
}

#endif