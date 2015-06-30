#include "config.h"

#ifndef __VFS_H__
#define __VFS_H__

struct vfs_h
{
	uint8_t reference_count;
};

void setup_vfs();
int vfs_open(char *filename, int mode);

#endif
