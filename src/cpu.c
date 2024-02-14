#include "cpu.h"

struct cpu cpu = {0};

void cpu_init(void)
{
	// bios
	cpu.pc = 0xbfc00000;
}

void cpu_cycle(void)
{
	uint32_t instr = cpu_fetch_instr(cpu.pc);
	cpu_exec(instr);
	cpu.pc += 4;
}