#include <stdlib.h>
#include <stdio.h>

#include "bist.h"

cpu_t *bist_cpu;
mmu_t *bist_mmu;

void run_1_insn(uint16_t insn)
{
	const uint16_t virt = 0500;
	mmu_abort_t abort;

	mmu_write_word(bist_mmu, virt, insn, &abort); // copy insn into memory
	cpu_poke_reg(bist_cpu, 7, virt);			  // set PC
	cpu_step(bist_cpu);							  // run
}

void assert_reg_eq(uint8_t reg, uint16_t expected, const char *id)
{
	uint16_t actual = cpu_peek_reg(bist_cpu, reg);

	if (actual != expected)
	{
		printf("%s: assertion failed on r%d (%06o != %06o)\n",
			id, reg, actual, expected);
	}
}

void assert_mem_wd_eq(size_t offset, uint16_t expected, const char *id)
{
	mmu_abort_t abort;
	uint16_t actual = mmu_read_word(bist_mmu, offset, &abort);

	if (actual != expected)
	{
		printf("%s: assertion failed on @%o (%06o != %06o)\n",
			id, offset, actual, expected);
	}
}

void bist_3_3_1()
{
	// ADD R2, R4
	cpu_poke_reg(bist_cpu, 2, 000002);
	cpu_poke_reg(bist_cpu, 4, 000004);
	run_1_insn(060204);
	assert_reg_eq(2, 000002, "3.3.1.2");
	assert_reg_eq(4, 000006, "3.3.1.2");

	// COMB R4
	cpu_poke_reg(bist_cpu, 4, 022222);
	run_1_insn(0105104);
	assert_reg_eq(4, 022155, "3.3.1.3");
}

void bist_3_3_2()
{
	mmu_abort_t abort;

	// CLR (R5)+
	mmu_write_word(bist_mmu, 030000, 0111116, &abort);

	cpu_poke_reg(bist_cpu, 5, 030000);
	run_1_insn(005025);
	assert_mem_wd_eq(030000, 000000, "3.3.2.1");

	// CLRB (R5)+
	mmu_write_word(bist_mmu, 030000, 0111116, &abort);
	cpu_poke_reg(bist_cpu, 5, 030000);
	run_1_insn(0105025);
	assert_mem_wd_eq(030000, 0111000, "3.3.2.2");

	// ADD (R2)+, R4
	mmu_write_word(bist_mmu, 0100002, 010000, &abort);
	cpu_poke_reg(bist_cpu, 2, 0100002);
	cpu_poke_reg(bist_cpu, 4, 010000);
	run_1_insn(062204);
	assert_reg_eq(2, 0100004, "3.3.2.3");
	assert_reg_eq(4, 020000, "3.3.2.3");
	assert_mem_wd_eq(0100002, 010000, "3.3.2.3");
}

void bist(cpu_t *cpu, mmu_t *mmu)
{
	bist_cpu = cpu;
	bist_mmu = mmu;

	bist_3_3_1();
	bist_3_3_2();

	bist_cpu = NULL;
	bist_mmu = NULL;

	printf("BIST complete\n");
}
