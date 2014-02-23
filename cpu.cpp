#include "cpu.h"

struct cpu
{
	mmu_t *mmu;

	uint16_t r[8];			// gp regs

	uint16_t ps;			// processor status word
};

struct cpu_control
{
	// instruction fields per processor handbook pp. A-9
	uint16_t bop;
	uint16_t uop;

	uint16_t sf, df;
	uint8_t sm, sr, dm, dr;
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

uint16_t _cpu_fetch(cpu_t *cpu, mmu_abort_t *abort)
{
	return mmu_read_word(cpu->mmu, cpu->r[7], abort);
}

void _cpu_decode(cpu_t *cpu, cpu_control *control)
{

}

void _cpu_execute(cpu_t *cpu, cpu_control *control)
{

}

void cpu_step(cpu_t *cpu)
{
	mmu_abort_t abort;
	cpu_control control;

	uint16_t insnWord = _cpu_fetch(cpu, &abort); // IF
	CHECK_ABORT(&abort);

	_cpu_decode(cpu, &control);
	_cpu_execute(cpu, &control);

mmu_abort:
	// TODO: actual abort handling code
	return;
}

#ifdef ARDUINO
String cpu_dump(cpu_t *cpu)
{

}
#endif