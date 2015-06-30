
#include "block_device_manager.h"
#include "simulated_device.h"
#include "mbr.h"

#include <stdio.h>

int main()
{
  block_device_manager manager;
  
  manager.register_type( &mbr.device_qualifies );
  
  // manager.register_device( "HD0", new ata_harddisk(&pci_device), true );
  block_device *bd = new simulated_device("testdisk.img");
  manager.register_device("testdisk.img", bd);
  
  printf("MBR: %d\n", mbr::device_qualifies(bd));
  
  //manager.spawn_children( "testdisk.img" );
  // Performs autodetection...
  //	First by device itself and then by the device manager itself
  //	Classes can register themselves and their autodetect functions
  // Creates partitions
  //	testdisk.img/0/0	testdisk.img/0/1
  
  
  //static int mbr::device_qualifies(...);
  // With int some kind of scoring-system
    
  //manager.register_type( fat );
  //manager.register_type( ext2 );
  
  return 0;
}
