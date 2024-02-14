#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "bios.h"

struct bios bios;

void bios_open(const char *filename)
{
	bios.fp = fopen(filename, "rb");
	if (bios.fp == NULL) {
		// @todo: display error and die
		return;
	}

	fseek(bios.fp, 0, SEEK_END);
	bios.sz = ftell(bios.fp);
	rewind(bios.fp);

	bios.data = malloc(bios.sz);
	fread(bios.data, bios.sz, 1, bios.fp);
}