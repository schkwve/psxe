#include "psxe.h"
#include "cpu.h"
#include "bios.h"
#include "devtree.h"

struct cpu cpu = {0};

void cpu_init(struct devtree devtree)
{
	cpu.devtree = devtree;

	for (int i = 0; i < 32; i++) {
		cpu.regs[i] = 0;
	}

	cpu.pc = BIOS_START;
	cpu.hi = 0;
	cpu.lo = 0;
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
	switch (instr >> 26) {
		case 0x0D: { // ori
			uint32_t reg = (instr >> 16) & 0xFF;
			uint32_t val = (instr & 0xFFFF);
			uint32_t res = cpu_get_reg(reg) | val;
			fprintf(stderr, "ori: (0x%x | $%d) -> %d (= 0x%x)\n", val, reg, reg, res);
			cpu_set_reg(reg, res);
			break;
		}
		case 0x0F: { // lui
			uint16_t reg = (instr >> 16) & 0x1F;
			uint32_t val = (instr & 0xFFFF) << 16;
			cpu_set_reg(reg, val);
			fprintf(stderr, "lui: 0x%x -> %d\n", val, reg);
			break;
		}
		default: {
			fprintf(stderr, "[psxe/error]: Unknown instruction! (0x%x)\n", instr);
			die();
			break;
		}
	}
}

void cpu_set_reg(uint8_t index, uint32_t val)
{
	cpu.regs[index] = val;
	cpu.regs[0] = 0;
}

uint32_t cpu_get_reg(uint8_t index)
{
	return cpu.regs[index];
}

void cpu_cycle(void)
{
	uint32_t instr = cpu_fetch_instr(cpu.pc);
	cpu_exec(instr);
	cpu.pc += 4;
}
