#ifndef ARDUINO

#include "mmu.h"
#include "cpu.h"

#include "bist.h"

mmu_t *mmu;
cpu_t *cpu;

int main(int argc, char **argv)
{
	mmu = mmu_create(131072);
	cpu = cpu_create(mmu);

	bist(cpu, mmu);
}

#endif /* !ARDUINO */