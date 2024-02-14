#ifndef BIOS_H
#define BIOS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// 512 kb
#define BIOS_SIZE (512 * 1024)

#define BIOS_START 0xBFC00000

#define BIOS_ADDR_IN_RANGE(x) ((x) >= BIOS_START && (x) < (BIOS_START + BIOS_SIZE))

struct bios {
	FILE *fp;
	size_t sz;
	uint8_t *data;
};

void bios_open(struct bios *bios, const char *filename);

uint32_t bios_load(struct bios *bios, uint32_t addr);

#endif /* BIOS_H */
