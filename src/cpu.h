#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "devtree.h"

struct cpu {
	uint32_t pc;

	struct devtree devtree;
};

void cpu_init(struct devtree devtree);
uint32_t cpu_fetch_instr(uint32_t addr);
void cpu_exec(uint32_t instr);

void cpu_cycle(void);

#endif /* CPU_H */
