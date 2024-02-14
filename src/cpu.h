#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "devtree.h"

struct cpu {
	// gpregs
	uint32_t regs[32];

	// sregs
	uint32_t pc;
	uint32_t hi;
	uint32_t lo;

	struct devtree devtree;
};

void cpu_init(struct devtree devtree);
uint32_t cpu_fetch_instr(uint32_t addr);
void cpu_exec(uint32_t instr);

void cpu_set_reg(uint8_t index, uint32_t val);
uint32_t cpu_get_reg(uint8_t index);

void cpu_cycle(void);

#endif /* CPU_H */
