
#include "SimulatedDevice.h"
#include "FileSystemExt2.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ext2.h"

int main()
{
    const char *filename = "../test/testfs-ext2";
    
    SimulatedDevice device(filename);    
    Extended2 ext2(&device);      
   
    File *newfile = ext2.createFile(2, (uint8_t *) "Directory");
    
    File *file = ext2.openFile(2);
    
    dirent_t de;
       
    uint32_t offset = 0;
    uint32_t num_files = 1024;
    
    while(offset < file->GetSize()) {
      file->Seek(offset);
      file->Read(sizeof(dirent_t), &de);
            
      printf("File at inode %d is named ", de.inode);
      for(int c = 0; c < de.name_len; c++) {
	printf("%c", de.name[c]);
      }
      printf("\n");
      
      offset += de.rec_len;     
    }
        
    return 0;
}
