#include "io.h"
#include "console.h"

uint16_t pci_read_word(uint16_t bus, uint16_t device, uint16_t function, uint16_t offset)
{
	uint32_t address;

	address = bus << 16;
	address |= device << 11;
	address |= function << 8;
	address |= offset & 0xFC;
	address |= 0x80000000;

	outl(address, 0xCF8);
	return (inl(0xCFC) >> (offset & 0x02) * 0x08) & 0xFFFF;
}
	
void print_pci_device_list()
{
	kc_print_string("Detecting PCI devices...\n");

	int bus, device;
	int max_bus = 2;

	for(bus = 0; bus < max_bus; bus++) {
		for(device = 0; device < 64; device++) {
			//int vendor = getcfgbyte(bus,device,0,PCI_HEADER_TYPE);;
			//int type = getcfgword(bus,device,0, PCI_VENDORID);
			int vendor = pci_read_word(bus, device, 0, 0);
			int type = pci_read_word(bus, device, 0, 2);

			if(vendor != 0x0000 && vendor != 0xFFFF) {
				int cls = pci_read_word(bus, device, 0, 10);

				kc_printf("PCI Device (Type=%x, Vendor=%x, Class=%x)\n", type, vendor, cls);
			}
		}
	}
	kc_printf("\n");
}
