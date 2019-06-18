#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <asm/ioctls.h>

unsigned char int2uc(int n)
{
	unsigned char c;
	switch (n){
		case 1: c=0xf9; break;
		case 2: c=0xA4; break;
		case 3: c=0xB0; break;
		case 4: c=0x99; break;
		case 5: c=0x92; break;
		case 6: c=0x82; break;
		case 7: c=0xF8; break;
		case 8: c=0x80; break;
		case 9: c=0x90; break;
		case 11: c=0xC0; break;
		default: c=0xff; break;
	}
	return c;
}

int uc2int(unsigned char n)
{
	int c;
	switch (n){
		case 0xC0: c=0; break;
		case 0xf9: c=1; break;
		case 0xA4: c=2; break;
		case 0xB0: c=3; break;
		case 0x99: c=4; break;
		case 0x92: c=5; break;
		case 0x82: c=6; break;
		case 0xF8: c=7; break;
		case 0x80: c=8; break;
		case 0x90: c=9; break;
		default: c=10; break;
	}
	return c;
}


