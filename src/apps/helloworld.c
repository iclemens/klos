
extern void _sys_write(unsigned char *string, unsigned int length);
extern unsigned int _sys_getpid();
extern unsigned int _sys_fork();
extern unsigned int _sys_exit();

unsigned int strlen(char *str)
{
	int n = 0;

	while(*(str++) != 0)
		n++;

	return n;
}

void print_string(char *str)
{
	_sys_write(str, strlen(str));
}

void print_hex(unsigned int nr)
{
	char kc_hex_chars[16];

	int j = 0;
	int i;

	for(i = 0; i < 10; i++)
		kc_hex_chars[i] = '0' + i;
	for(i = 10; i < 16; i++)
		kc_hex_chars[i] = 'A' + i - 10;

	_sys_write("0", 1);
	_sys_write("x", 1);

	if(nr == 0) {
		_sys_write("0", 1);
		return;
	}

	for(i = 7; i >= 0; i--) {
		unsigned int tmp = (nr >> (i * 4)) & 0xF;

		if(j == 0 && tmp != 0)
			j = 1;

		if(j != 0)
			_sys_write(&(kc_hex_chars[tmp]), 1);
	}
}

int main()
{
	int i;

	unsigned int pid = _sys_getpid();

	print_string("\nMy process ID is: ");
	print_hex(pid);
	print_string("\n");

	unsigned int frk = _sys_fork();

	print_string("\n");
	print_string("Fork returned: ");
	print_hex(frk);
	pid = _sys_getpid();
	print_string("("); print_hex(pid);
	print_string(")\n");

	return 0;
}
