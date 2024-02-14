#ifndef DEVTREE_H
#define DEVTREE_H

#include "bios.h"
#include "memory.h"

struct devtree {
	struct bios bios;
	struct memory ram;
};

#endif /* DEVTREE_H */
