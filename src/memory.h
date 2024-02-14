#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEMCTL_ADDR_IN_RANGE(x) (((x) >= 0x1F801000) && ((x) < 0x1F801000 + 0x24))
#define MEM_RAMSIZE_CONFIG_ADDR_RANGE(x) (((x) >= 0x1F801060) && ((x) < 0x1F801060 + 0x04))

#define MEM_EXP1_BASE_ADDR (0x1F000000)
#define MEM_EXP2_BASE_ADDR (0x1F802000)

uint32_t memory_get(uint32_t addr);
void memory_set(uint32_t addr, uint32_t val);

#endif /* MEMORY_H */
