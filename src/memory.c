#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "psxe.h"

void memory_init(struct memory *mem)
{
	size_t length = sizeof(mem->data) / sizeof(mem->data[0]);
	for (size_t i = 0; i < length; i++) {
		mem->data[i] = 0xAB;
	}
}

uint32_t memory_get(uint32_t addr)
{
	if (cpu_is_cache_isolated()) {
		fprintf(stderr, "[psxe/info]: Ignoring read from 0x%x (cache is isolated)\n", addr);
		return 0;
	}

	if (addr % 4 != 0) {
		fprintf(stderr, "unaligned get\n");
		die();
	}

	if (BIOS_ADDR_IN_RANGE(addr)) {
		return bios_load(&cpu.devtree.bios, addr);
	} else if (MEMCTL_ADDR_IN_RANGE(addr)) {
		uint8_t offset = addr - 0x1f801000;
		if (offset == 0) {
			return MEM_EXP1_BASE_ADDR;
		} else if (offset == 4) {
			return MEM_EXP2_BASE_ADDR;
		} else {
			fprintf(stderr, "[psxe/error]: Unhandled write to MEMCTL\n");
		}
	} else if (MEM_RAMSIZE_CONFIG_ADDR_RANGE(addr)) {
		// ignore
	} else if (MEM_RAM_ADDR_IN_RANGE(addr)) {
		uint32_t a = cpu.devtree.ram.data[addr];
		uint32_t b = cpu.devtree.ram.data[addr + 1];
		uint32_t c = cpu.devtree.ram.data[addr + 2];
		uint32_t d = cpu.devtree.ram.data[addr + 3];
		return (a | (b << 8) | (c << 16) | (d << 24));
	}

	return 0;
}

void memory_set(uint32_t addr, uint32_t val)
{
	if (cpu_is_cache_isolated()) {
		fprintf(stderr, "[psxe/info]: Ignoring write 0x%x to 0x%x (cache is isolated)\n", val, addr);
		return;
	}

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
	} else if (MEM_RAM_ADDR_IN_RANGE(addr)) {
		cpu.devtree.ram.data[addr] = (uint8_t)val;
		cpu.devtree.ram.data[addr + 1] = (uint8_t)(val >> 8);
		cpu.devtree.ram.data[addr + 2] = (uint8_t)(val >> 16);
		cpu.devtree.ram.data[addr + 3] = (uint8_t)(val >> 24);
	}
}
