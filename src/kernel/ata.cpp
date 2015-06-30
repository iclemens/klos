#include "ata.h"
#include "config.h"
#include "kalloc.h"
#include "io.h"
#include "console.h"

#define ATA_ERR  0x01
#define ATA_IDX	 0x02
#define ATA_CORR 0x04
#define ATA_DRQ  0x08
#define ATA_DSC	 0x10
#define ATA_DF   0x20
#define ATA_DRDY 0x40
#define ATA_BSY  0x80

struct ata_device_t {
	uint16_t base_register;
	uint16_t device_id;

	// 1 - LBA48
	// 2 - ATAPI
	uint16_t flags;

	struct ata_device_t *next_device;
};

struct ata_device_t *ata_device_list = NULL;

/**
 * Adds an ATA device to the ata_device_list.
 */
struct ata_device_t *register_ata_device(uint16_t base_register, uint16_t device_id)
{
	struct ata_device_t *tmp = ata_device_list;

	ata_device_list = (struct ata_device_t *) kalloc(sizeof(struct ata_device_t));
	ata_device_list->base_register = base_register;
	ata_device_list->device_id = device_id;
	ata_device_list->next_device = tmp;

	return ata_device_list;
}

/**
 * Checks if an ATA controller exists at the specified address
 * and adds all attached drives to the ata_device_list.
 */
void setup_ata_controller(uint16_t base_register)
{
	/* A very crude way to detect whether the
			controller exists, 0xBA is just another
			random value */
	outb(0xBA, base_register + 3);

	if(inb(base_register + 3) == 0xBA) {
		/* Check for first harddisk */
		outb(0xA0, base_register + 6);

		if(inb(base_register + 7) & 0x40)
			register_ata_device(base_register, 0);

		/* Check for second harddisk*/
		outb(0xB0, base_register + 6);

		if(inb(base_register + 7) & 0x40)
			register_ata_device(base_register, 1);
	}
}

/**
 * Searches the primary and secondary ATA
 * controllers for devices and sets them up.
 * FIXME: Should decouple this from the ATA 
 * driver itself
 */   
void setup_ata_devices()
{
	//kc_printf("Setting up ATA devices...\n");

	/* Primary controller */
	setup_ata_controller(0x1F0);

	/* Secondary controller */
	setup_ata_controller(0x170);

	struct ata_device_t *tmp;

	for(tmp = ata_device_list; tmp != NULL; tmp = tmp->next_device) {
		//kc_printf("ATA Device (Base=%x, Device=%x)\n", tmp->base_register, tmp->device_id);
	}
}

void ata_get_device_info(struct ata_device_t *device, char *buffer)
{
	int i;
	int tmp;

	outb(0, device->base_register + 0x01);
	outb(0, device->base_register + 0x02);
	outb(0, device->base_register + 0x03);
	outb(0, device->base_register + 0x04);
	outb(0, device->base_register + 0x05);
	outb(device->device_id << 4, device->base_register + 0x06);
	outb(0xEC, device->base_register + 0x07);

	/* FIXME: This blocks the kernel */
	while(!(inb(0x1F7) & 0x08)) {};

	for(i = 0; i < 256; i++) {
		tmp = inw(0x1F0);
		buffer[i * 2 + 1] = tmp & 0xFF;
		buffer[i * 2] = (tmp >> 8) & 0xFF;
	}
}

void ata_pio_read(struct ata_device_t *device, unsigned char *buffer, int sector, int length)
{
	int i;
	int tmp;
	int addr = 0;

	// Read sector
	// Features, sect. ,lba low mid high, dev, cmd 
	outb(0x00, 0x1F1);	
	outb(length >> 9, 0x1F2);	/* 1 Sector */
	outb(sector & 0xFF, 0x1F3);	/* 7-0 */
	outb((sector >> 8) & 0xFF, 0x1F4);	/* 15-8 */
	outb((sector >> 8) & 0xFF, 0x1F5); /* 23-16 */
	outb(0xE0 | (device->device_id << 4) | ((sector >> 24) & 0x0F), 0x1F6); /* 27-24 */
	outb(0xE0, 0x1F6);
	outb(0x20, 0x1F7);

	//kc_print_string("Waiting for command to complete\n");

	int status = inb(0x1F7);

	if(status & ATA_ERR || status & ATA_DF) {
		kc_print_string("Warning: error bit set or device fault\n");
		kc_print_string(" Status: ");
		kc_print_hex(status);
		kc_print_string("\n");
	}

	while(!(status & ATA_DRQ)) {
		status = inb(0x1F7);
	}

	//kc_print_string("Done. Loading data...\n");

	for(i = 0; i < (length >> 1); i++) {
		tmp = inw(0x1F0);
		//buffer[i * 2 + 1] = tmp & 0xFF;
		//buffer[i * 2] = (tmp >> 8) & 0xFF;
		buffer[i * 2] = tmp & 0xFF;
		buffer[i * 2 + 1] = (tmp >> 8) & 0xFF;
	}

	//kc_print_string("\nData:\n");

	for(i = 0; i < 10; i++) {
		//kc_print_hex(buffer[i]);
		//kc_print_string("\n");
	}
}

void ata_read(int device, unsigned char *buffer, int sector, int length)
{
	ata_pio_read(ata_device_list, buffer, sector, length);
}

/*	
	int drive = 0;
	int tmp;

	outb(0, 0x1F1);
	outb(0, 0x1F2);
	outb(0, 0x1F3);
	outb(0, 0x1F4);
	outb(0, 0x1F5);
	outb(drive << 4, 0x1F6);
	outb(0xEC, 0x1F7);

	//Read = 0x20

	while(!(inb(0x1F7) & 0x8)) {};

	int i;
	char buffer[512];

	for(i = 0; i < 256; i++) {
		tmp = inw(0x1F0);
		buffer[i * 2 + 1] = tmp & 0xFF;
		buffer[i * 2] = (tmp >> 8) & 0xFF;
	}

	if(buffer[83 * 2 + 1] & 0x04)
		kc_print_string("LBA48 Supported\n");
	else
		kc_print_string("LBA48 NOT Supported\n");

	kc_print_string("Serial: ");
	for(i = (10 * 2); i < (20 * 2); i++)
		kc_write_character(buffer[i]);
	kc_print_string("\n");

	kc_print_string("Model: ");
	for(i = (27 * 2); i < (47 * 2); i++)
		kc_write_character(buffer[i]);
	kc_print_string("\n");

	int addr = 1;

	kc_print_string("\nReading sector 1\n");

	// Read sector
	// Features, sect. ,lba low mid high, dev, cmd 
	outb(0x00, 0x1F1);	
	outb(0x01, 0x1F2);	/* 1 Sector */
	//outb(0x00, 0x1F3);	/* 7-0 */
	//outb(0x00, 0x1F4);	/* 15-8 */
	//outb(0x00, 0x1F5); /* 23-16 */
	//outb(0xE0 | (drive << 4) | ((1 >> 24) & 0x0F), 0x1F6); /* 27-24 */
	/*outb(0xE0, 0x1F6);
	outb(0x20, 0x1F7);

	kc_print_string("Waiting for command to complete\n");

	int status = inb(0x1F7);

	if(status & ATA_ERR || status & ATA_DF) {
		kc_print_string("Warning: error bit set or device fault\n");
		kc_print_string(" Status: ");
		kc_print_hex(status);
		kc_print_string("\n");
	}

	while(!(status & ATA_DRQ)) {
		status = inb(0x1F7);
	}

	kc_print_string("Done, loading data\n");

	for(i = 0; i < 256; i++) {
		tmp = inw(0x1F0);
		buffer[i * 2] = tmp & 0xFF;
		buffer[i * 2 + 1] = (tmp >> 8) & 0xFF;
	}

	kc_print_string("\nData:\n");

	for(i = 0; i < 512; i++)
		kc_write_character(buffer[i]);

*/
