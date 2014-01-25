#include <Arduino.h>

#include "mmu.h"

#ifndef _CPU_H
#define _CPU_H

class Cpu 
{
public:
	Cpu(Mmu *mmu);
	~Cpu();

 	uint16_t getReg(uint8_t reg);
 	void loadReg(uint8_t reg, uint16_t value);

 	void step();

 	String dump() const;

private:
	// implementation-independent processor state
	uint16_t m_r[8];
	uint16_t& m_sp = m_r[6];
	uint16_t& m_pc = m_r[7];

	uint16_t m_psw;

	Mmu *m_mmu;

	// implementation-specific processor state
	enum MicroOp 
	{
	};

	enum OperandLocation 
	{
		OpLoc_None,
		OpLoc_Reg,
		OpLoc_Mem,
		OpLoc_Imm
	};

	struct Control 
	{
		Control();

		MicroOp m_uop;

		OperandLocation m_Aloc;
		size_t m_Aaddr;

		OperandLocation m_Bloc;
		size_t m_Baddr;

		OperandLocation m_Zloc;
		size_t m_Zaddr;

		uint16_t m_result;
		uint16_t m_psw;
	};

	// internal helper methods
	void decode(Control *control);
	void execute(const Control *control);
	void writeBack(const Control *control);
};

#endif /* _CPU_H */