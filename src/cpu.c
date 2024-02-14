#include "psxe.h"
#include "cpu.h"
#include "bios.h"
#include "devtree.h"

struct cpu cpu = {0};

void cpu_init(struct devtree devtree)
{
	// bios
	cpu.pc = BIOS_START;
	cpu.devtree = devtree;
}

uint32_t cpu_fetch_instr(uint32_t addr)
{
	if (BIOS_ADDR_IN_RANGE(addr)) {
		return bios_load(&cpu.devtree.bios, addr);
	}
	die();
	return 0;
}

void cpu_exec(uint32_t instr)
{
	switch (instr) {
		default: {
			fprintf(stderr, "[psxe/error]: Unknown instruction! (0x%x)\n", instr);
			die();
			break;
		}
	}
}

void cpu_cycle(void)
{
	uint32_t instr = cpu_fetch_instr(cpu.pc);
	cpu_exec(instr);
	cpu.pc += 4;
}
