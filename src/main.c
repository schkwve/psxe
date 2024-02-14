#include <stdlib.h>
#include <stdio.h>

#include "psxe.h"
#include "cpu.h"
#include "bios.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "[psxe/info]: No BIOS specified!\n");
		return 1;
	}
	struct devtree devtree = {0};
	
	struct bios bios;
	bios_open(&bios, argv[1]);
	devtree.bios = bios;
	
	cpu_init(devtree);

	for (;;) {
		cpu_cycle();
	}

	return 0;
}

void die(void)
{
	exit(1);
}
