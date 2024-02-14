#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bios.h"
#include "memory.h"

void bios_open(struct bios *bios, const char *filename)
{
	bios->fp = fopen(filename, "rb");
	if (bios->fp == NULL) {
		// @todo: display error and die
		return;
	}

	bios->sz = BIOS_SIZE;

	bios->data = malloc(bios->sz);
	memset(bios->data, 0, bios->sz);
	fread(bios->data, bios->sz, 1, bios->fp);
}

uint32_t bios_load(struct bios *bios, uint32_t addr)
{
	uint32_t offset = addr - BIOS_START;
	uint8_t a = bios->data[offset];
	uint8_t b = bios->data[offset + 1];
	uint8_t c = bios->data[offset + 2];
	uint8_t d = bios->data[offset + 3];
	return (a | (b << 8) | (c << 16) | (d << 24));
}
