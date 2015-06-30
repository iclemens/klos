#ifndef __ATA_H_
#define __ATA_H_

void setup_ata_devices();
void ata_read(int device, unsigned char *buffer, int sector, int length);

#endif
