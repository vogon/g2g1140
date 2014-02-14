#include "cpu.h"

struct cpu
{
	mmu_t *mmu;

	uint16_t r[8];			// gp regs
	uint16_t &sp = r[6];	// alias for sp
	uint16_t &pc = r[7];	// alias for pc

	uint16_t ps;			// processor status word
};

cpu_t *cpu_create(mmu_t *mmu)
{
	cpu_t *cpu = (cpu_t *)malloc(sizeof(cpu_t));
	
	cpu->mmu = mmu;
	cpu_reset(cpu);
}

void cpu_destroy(cpu_t *cpu)
{
	free(cpu);
}

void cpu_reset(cpu_t *cpu)
{
	for (int i = 0; i < 8; i++)
	{
		cpu->r[i] = 0;
	}

	cpu->ps = 0;
}

uint16_t cpu_peek_reg(cpu_t *cpu, uint8_t reg)
{
	return cpu->r[reg];
}

void cpu_poke_reg(cpu_t *cpu, uint8_t reg, uint16_t value)
{
	cpu->r[reg] = value;
}

void cpu_step(cpu_t *cpu)
{

}

String cpu_dump(cpu_t *cpu)
{

}
