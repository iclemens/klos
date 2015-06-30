#include "console.h"
#include "io.h"
#include "config.h"

/**
 * This is a very basic driver for the text inferface.
 * What we need is a nice kprintf like function...
 */

// Base address of text area, will be moved after we've enabled paging
unsigned char *kc_text_base;

// Screen dimensions (in characters)
uint8_t kc_screen_width;
uint8_t kc_screen_height;

// Cursor position
uint8_t kc_x;
uint8_t kc_y;

// List of hexadecimal characters used by print_hex
char kc_hex_chars[16];

/**
 * Initializes the console
 */
void setup_console()
{
	int i;
	kc_text_base = (unsigned char *) 0xB8000;

	kc_screen_width = 80;
	kc_screen_height = 25;

	for(i = 0; i < 10; i++)
		kc_hex_chars[i] = '0' + i;
	for(i = 10; i < 16; i++)
		kc_hex_chars[i] = 'A' + i - 10;
	//kc_hex_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	kc_clear();
}

/**
 * Resets text base area
 */
void kc_set_text_base(uint32_t base)
{
	kc_text_base = (unsigned char *) base;
}

/**
 * Moves the hardware cursor to location x, y
 */
void kc_move_cursor(int x, int y)
{
	uint16_t position = y * kc_screen_width + x;

	outb(0x0E, 0x3D4);
	outb((position >> 8) & 0xFF, 0x3D5);
	outb(0x0F, 0x3D4);
	outb(position & 0xFF, 0x3D5);
}

/**
 * Clear the console
 */
void kc_clear()
{
	int i;

	for(i = 0; i < kc_screen_width * kc_screen_height; i++) {
		kc_text_base[i * 2 + 0] = 0;
		kc_text_base[i * 2 + 1] = 7;
	}

	kc_x = 0; kc_y = 0;
	kc_move_cursor(kc_x, kc_y);
}

/**
 * Scrolls the screen one line up
 */
void kc_scroll()
{
	int i, j, k;

	for(i = 1; i < kc_screen_height; i++) {
		for(k = 0; k < kc_screen_width; k++) {
			kc_text_base[((i - 1) * kc_screen_width + k) * 2 + 0] =
				kc_text_base[((i) * kc_screen_width + k) * 2 + 0];
			kc_text_base[((i - 1) * kc_screen_width + k) * 2 + 1] =
				kc_text_base[((i) * kc_screen_width + k) * 2 + 1];
		}
	}

	for(k = 0; k < kc_screen_width; k++) {
		kc_text_base[kc_screen_height * kc_screen_width * 2 - 2 * kc_screen_width + k * 2 + 0] = 0;
		kc_text_base[kc_screen_height * kc_screen_width * 2 - 2 * kc_screen_width + k * 2 + 1] = 7;
	}
}

/**
 * Write a character to the screen and advances the cursor
 */
void kc_write_character(char c)
{
	if(c == '\n') {
		kc_x = 0;
		kc_y++;
	} else if(c == '\t') {
		while(kc_x % 8 != 0)
			kc_write_character(' ');
	} else {
		kc_text_base[(kc_x + kc_y * kc_screen_width) * 2 + 0] = c;
		kc_text_base[(kc_x + kc_y * kc_screen_width) * 2 + 1] = 7;
		kc_x++;
	}

	if(kc_x >= kc_screen_width) {
		kc_x = 0;		
		kc_y++;
	}

	if(kc_y > kc_screen_height - 1) {
		kc_y--;
		kc_scroll();
	}

	kc_move_cursor(kc_x, kc_y);
}

void kc_print_integer(unsigned int nr)
{
	int i = 1000000000, j = 0;
	int s;

	if(nr == 0) {
		kc_write_character('0');
		return;
	}

	while(i > 0) {
		s = nr / i;
		nr -= (s * i);

		if(j == 0 && s != 0)
			j = 1;

		if(j != 0)
			kc_write_character('0' + s);

		i /= 10;
	}
}

void kc_print_hex(unsigned int nr)
{
	int j = 0;
	int i;

	kc_write_character('0');
	kc_write_character('x');

	if(nr == 0) {
		kc_write_character('0');
		return;
	}

	for(i = 7; i >= 0; i--) {
		unsigned int tmp = (nr >> (i * 4)) & 0xF;

		if(j == 0 && tmp != 0)
			j = 1;

		if(j != 0)
			kc_write_character(kc_hex_chars[tmp]);
	}
}

/**
 * Prints a string to the screen
 */
void kc_print_string(const char *string)
{  
	while(*string != 0x00) {
		kc_write_character(*string);
		string++;
	}
}

void kc_printf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	while(*fmt) {
		if(*fmt == '%') {
			fmt++;

			if(*fmt == NULL) 
				continue;

			switch(*fmt) 
			{
				case 'd': {
						int value = va_arg(ap, int);
						kc_print_integer(value);
					}
					break;
				case 'x': {
						int value = va_arg(ap, int);
						kc_print_hex(value);
					}
					break;
				case 's': {
						char *value = va_arg(ap, char*);
						kc_print_string(value);
					}
					break;
			}

		} else
			kc_write_character(*fmt);

		fmt++;
	}
	
	va_end(ap);
}
