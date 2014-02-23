#include <stdio.h>

#include "cpu.h"

struct cpu
{
	mmu_t *mmu;

	uint16_t r[8];			// gp regs

	uint16_t ps;			// processor status word
};

struct cpu_control
{
	cpu_control() : bop(0), uop(0), sm(0), sr(0), dm(0), dr(0) {}

	// instruction fields per processor handbook pp. A-9
	uint16_t bop;
	uint16_t uop;

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
	uint16_t insnWord = mmu_read_word(cpu->mmu, cpu->r[7], abort);
	printf("_cpu_fetch: fetched %06o\n", insnWord);

	return insnWord;
}

void _cpu_decode(cpu_t *cpu, uint16_t insnWord, cpu_control *control)
{
	control->bop = insnWord & 0xf000;

	control->sm = (insnWord & 007000) >> 9;
	control->sr = (insnWord & 000700) >> 6;

	control->sm = (insnWord & 000070) >> 3;
	control->sr = (insnWord & 000007) >> 0;

	control->uop = insnWord & 0xffc0;
}

void _cpu_execute(cpu_t *cpu, cpu_control *control)
{
	bool done = false;

	// unary operations
	switch(control->uop)
	{
	case 0005000:
	case 0105000:
		// CLR(B)
		printf("CLR/CLRB\n");
		break;
	case 0005100:
	case 0105100:
		// COM(B)
		printf("COM/COMB\n");
		break;
	case 0005200:
	case 0105200:
		// INC(B)
		printf("INC/INCB\n");
		break;
	case 0005300:
	case 0105300:
		// DEC(B)
		printf("DEC/DECB\n");
		break;
	}

	if (done) return;

	// binary operations
	switch (control->bop)
	{
	case 0010000:
	case 0110000:
		// MOV(B)
		printf("MOV/MOVB\n");
		break;
	case 0020000:
	case 0120000:
		// CMP(B)
		printf("CMP/CMPB\n");
		break;
	case 0060000:
		// ADD
		printf("ADD\n");
		break;
	case 0160000:
		// SUB
		printf("SUB\n");
		break;
	case 0030000:
	case 0130000:
		// BIT(B)
		printf("BIT/BITB\n");
		break;
	case 0040000:
	case 0140000:
		// BIC(B)
		printf("BIC/BICB\n");
		break;
	case 0050000:
	case 0150000:
		// BIS(B)
		printf("BIS/BISB\n");
		break;
	}

	if (done) return;
}

void cpu_step(cpu_t *cpu)
{
	mmu_abort_t abort;
	cpu_control control;

	uint16_t insnWord = _cpu_fetch(cpu, &abort); // IF
	CHECK_ABORT(&abort);

	_cpu_decode(cpu, insnWord, &control);
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