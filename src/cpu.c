#include "psxe.h"
#include "cpu.h"
#include "bios.h"
#include "memory.h"
#include "devtree.h"

struct cpu cpu;

void cpu_init(struct devtree devtree)
{
	cpu.devtree = devtree;

	for (int i = 0; i < 32; i++) {
		cpu.regs[i] = 0;
	}

	cpu.pc = BIOS_START;
	cpu.hi = 0;
	cpu.lo = 0;

	cpu.cop0_sr = 0;

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
				case 0x25: { // or
					uint32_t a = (instr >> 21) & 0x1F;
					uint32_t b = (instr >> 16) & 0x1F;
					uint32_t dest = (instr >> 11) & 0x1F;
					uint32_t res = cpu_get_reg(a) | cpu_get_reg(b);
					fprintf(stderr, "or: ($%d | $%d) -> $%d (= 0x%x)\n", a, b, dest, res);
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
		case 0x05: { // bne
			int16_t offset = (instr & 0xFFFF);
			uint32_t a = (instr >> 16) & 0x1F;
			uint32_t b = (instr >> 21) & 0x1F;

			if (cpu_get_reg(a) != cpu_get_reg(b)) {
				cpu_branch((uint32_t)offset);
			}
			fprintf(stderr, "bne: ($%d != $%d) => pc%d\n", a, b, (uint32_t)offset);
			break;
		}
		case 0x08: { // addi
			int16_t val = (instr & 0xFFFF);
			uint32_t src = (instr >> 21) & 0x1F;
			uint32_t dest = (instr >> 11) & 0x1F;
			int64_t res = (int64_t)((int32_t)cpu_get_reg(src) + (int32_t)val);
			fprintf(stderr, "addi: ($%d + 0x%x) -> $%d (= 0x%llx)\n", src, val, dest, res);
			if (res < INT32_MIN || res > INT32_MAX) {
				// trigger an exception
				fprintf(stderr, "+++ EXCEPTION TRIGGERED +++\n");
				die();
				break;
			}
			cpu_set_reg(dest, (int32_t)res);
			break;
		}
		case 0x09: { // addiu
			int16_t val = (instr & 0xFFFF);
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
		case 0x10: { // mtc0
			uint32_t src = (instr >> 16) & 0x1F;
			uint32_t dest = (instr >> 11) & 0x1F;
			fprintf(stderr, "mtc0: $%d -> $cop0_%d\n", src, dest);
			cpu.cop0_sr = cpu_get_reg(src);
			break;
		}
		case 0x23: { //lw
			int16_t offset = (instr & 0xFFFF);
			uint32_t src = (instr >> 16) & 0x1F;
			uint32_t dest = (instr >> 21) & 0x1F;
			uint32_t addr = cpu_get_reg(src) + (uint32_t)offset;
			uint32_t word = memory_get(addr);
			fprintf(stderr, "lw: ($%d + %d) -> $%d\n", src, (uint32_t)offset, dest);
			cpu_set_reg(dest, word);
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

void cpu_branch(uint32_t offset)
{
	offset <<= 2;

	uint32_t new_pc = cpu.pc;
	new_pc += offset;
	new_pc += 4;
	cpu.pc = new_pc;
}

uint8_t cpu_is_cache_isolated(void)
{
	return (cpu.cop0_sr & 0x10000) != 0;
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
