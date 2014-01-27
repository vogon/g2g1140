#include "bist.h"

Cpu *bist_cpu;
Mmu *bist_mmu;

void run_1_insn(uint16_t insn)
{
	const uint16_t virt = 0500;
	Mmu::Abort abort;

	bist_mmu->write(virt, insn, &abort); 	// copy insn into memory
	bist_cpu->loadReg(7, virt);				// set PC
	bist_cpu->step();						// run
}

void assert_reg_eq(uint8_t reg, uint16_t expected, String id)
{
	uint16_t actual = bist_cpu->getReg(reg);

	if (actual != expected)
	{
		Serial.print(id);
		Serial.print(": assertion failed on r");
		Serial.print(reg);
		Serial.print(" (");
		Serial.print(actual, OCT);
		Serial.print(" != ");
		Serial.print(expected, OCT);
		Serial.println(")");
	}
}

void assert_mem_wd_eq(size_t offset, uint16_t expected, String id)
{
	Mmu::Abort abort;
	uint16_t actual = bist_mmu->read(offset, &abort);

	if (actual != expected)
	{
		Serial.print(id);
		Serial.print(": assertion failed on @");
		Serial.print(offset, OCT);
		Serial.print(" (");
		Serial.print(actual, OCT);
		Serial.print(" != ");
		Serial.print(expected, OCT);
		Serial.println(")");
	}
}

void bist_3_3_1()
{
	// ADD R2, R4
	bist_cpu->loadReg(2, 000002);
	bist_cpu->loadReg(4, 000004);
	run_1_insn(060204);
	assert_reg_eq(2, 000002, "3.3.1.2");
	assert_reg_eq(4, 000006, "3.3.1.2");

	// COMB R4
	bist_cpu->loadReg(4, 022222);
	run_1_insn(0105104);
	assert_reg_eq(4, 022155, "3.3.1.3");
}

void bist_3_3_2()
{
	Mmu::Abort abort;

	// CLR (R5)+
	bist_mmu->write(030000, 0111116, &abort);

	bist_cpu->loadReg(5, 030000);
	run_1_insn(005025);
	assert_mem_wd_eq(030000, 000000, "3.3.2.1");

	// CLRB (R5)+
	bist_mmu->write(030000, 0111116, &abort);
	bist_cpu->loadReg(5, 030000);
	run_1_insn(0105025);
	assert_mem_wd_eq(030000, 0111000, "3.3.2.2");

	// ADD (R2)+, R4
	bist_mmu->write(0100002, 010000, &abort);
	bist_cpu->loadReg(2, 0100002);
	bist_cpu->loadReg(4, 010000);
	run_1_insn(062204);
	assert_reg_eq(2, 0100004, "3.3.2.3");
	assert_reg_eq(4, 020000, "3.3.2.3");
	assert_mem_wd_eq(0100002, 010000, "3.3.2.3");
}

void bist(Cpu *cpu, Mmu *mmu)
{
	bist_cpu = cpu;
	bist_mmu = mmu;

	bist_3_3_1();
	bist_3_3_2();

	bist_cpu = NULL;
	bist_mmu = NULL;

	Serial.println("BIST complete");
}