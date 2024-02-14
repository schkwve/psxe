#include "psxe.h"
#include "cpu.h"
#include "bios.h"
#include "memory.h"
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

	cpu.cur_instr = 0;
	cpu.next_instr = 0;
}

uint32_t cpu_fetch_instr(uint32_t addr)
{
	return memory_get(addr);
}

void cpu_exec(uint32_t instr)
{
	// list of implemented opcodes:
	//
	// - sll
	// - ori
	// - lui
	// - sw 
	//

	switch (instr >> 26) {
		case 0x00: { // special
			switch (instr & 0xFF) {
				case 0x00: { // sll
					uint32_t val = (instr >> 6) & 0x1F;
					uint32_t src = (instr >> 21) & 0x1F;
					uint32_t dest = (instr >> 11) & 0x1F;
					uint32_t res = cpu_get_reg(src) << val;
					fprintf(stderr, "sll: ($%d << $%d) -> $%d (= 0x%x)\n", src, val, dest, res);
					cpu_set_reg(dest, res);
					break;
				}
			}
			break;
		}
		case 0x02: { // J
			uint32_t addr = (instr & 0x3ffffff);
			uint32_t new_pc = (cpu.pc & 0xf0000000) | (addr << 2);
			fprintf(stderr, "J: => 0x%x\n", new_pc);
			cpu.pc = new_pc;
			break;
		}
		case 0x09: { // addiu
			uint16_t val = (instr & 0xFFFF);
			uint32_t src = (instr >> 21) & 0x1F;
			uint32_t dest = (instr >> 11) & 0x1F;
			uint32_t res = cpu_get_reg(src) + (uint32_t)val;
			fprintf(stderr, "addiu: ($%d + 0x%x) -> $%d (= 0x%x)\n", src, val, dest, res);
			cpu_set_reg(dest, res);
			break;
		}
		case 0x0D: { // ori
			uint32_t reg = (instr >> 16) & 0x1F;
			uint32_t val = (instr & 0xFFFF);
			uint32_t res = cpu_get_reg(reg) | val;
			fprintf(stderr, "ori: (0x%x | $%d) -> $%d (= 0x%x)\n", val, reg, reg, res);
			cpu_set_reg(reg, res);
			break;
		}
		case 0x0F: { // lui
			uint32_t reg = (instr >> 16) & 0x1F;
			uint32_t val = (instr & 0xFFFF) << 16;
			fprintf(stderr, "lui: 0x%x -> $%d\n", val, reg);
			cpu_set_reg(reg, val);
			break;
		}
		case 0x2B: { // sw
			uint32_t reg_val = (instr >> 16) & 0x1F;
			uint32_t reg_addr = ((instr >> 21) & 0x1F);
			int16_t offset = (instr & 0xFFFF);
			uint32_t addr = cpu_get_reg(reg_addr) + (uint32_t)offset;
			fprintf(stderr, "sw: ($%d + 0x%x) = $%d\n", reg_addr, (uint32_t)offset, reg_val);
			memory_set(addr, cpu_get_reg(reg_val));
			break;
		}
		default: {
			fprintf(stderr, "[psxe/fatal]: Unknown instruction! (0x%x)\n", instr);
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
	cpu.cur_instr = cpu.next_instr;
	cpu.next_instr = cpu_fetch_instr(cpu.pc);
	cpu_exec(cpu.cur_instr);
	cpu.pc += 4;
}
