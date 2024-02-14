#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "psxe.h"

uint32_t memory_get(uint32_t addr)
{
	if (addr % 4 != 0) {
		fprintf(stderr, "unaligned get\n");
		die();
	}

	if (BIOS_ADDR_IN_RANGE(addr)) {
		return bios_load(&cpu.devtree.bios, addr);
	}

	die();
	return 0;
}

void memory_set(uint32_t addr, uint32_t val)
{
	if (addr % 4 != 0) {
		fprintf(stderr, "unaligned store\n");
		die();
	}

	if (BIOS_ADDR_IN_RANGE(addr)) {
		fprintf(stderr, "[psxe/fatal]: Tried to overwrite BIOS!\n");
		die();
	} else if (MEMCTL_ADDR_IN_RANGE(addr)) {
		uint8_t offset = addr - 0x1f801000;
		if (offset == 0) {
			if (val != MEM_EXP1_BASE_ADDR) {
				fprintf(stderr, "[psxe/fatal]: Tried to write invalid value to EXP1!\n");
				die();
			}
		} else if (offset == 4) {
			if (val != MEM_EXP2_BASE_ADDR) {
				fprintf(stderr, "[psxe/fatal]: Tried to write invalid value to EXP2!\n");
				die();
			}
		} else {
			fprintf(stderr, "[psxe/error]: Unhandled write to MEMCTL\n");
		}
	} else if (MEM_RAMSIZE_CONFIG_ADDR_RANGE(addr)) {
		// ignore
	}
}
