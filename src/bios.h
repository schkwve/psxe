#ifndef BIOS_H
#define BIOS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct bios {
	FILE *fp;
	size_t sz;
	uint8_t *data;
};

void bios_open(const char *filename);

#endif /* BIOS_H */
