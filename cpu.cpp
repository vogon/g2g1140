#include "cpu.h"

Cpu::Cpu(Mmu *mmu) 
{
	m_mmu = mmu;

	for (int i = 0; i < 8; i++)
	{
		m_r[i] = 0;
	}

	m_psw = 0;
}

Cpu::~Cpu()
{
}

Cpu::Control::Control()
{
}

void Cpu::loadReg(uint8_t reg, uint16_t value)
{
	m_r[reg] = value;
}

void Cpu::step()
{
	Control control;

	decode(&control);
	execute(&control);
	writeBack(&control);
}

void Cpu::decode(Control *control)
{
	Mmu::Abort abort;
	uint16_t insn = m_mmu->read(m_pc, &abort);

	if (insn & 0070000 == 0)
	{
		// single-operand
	}
	else
	{
		// double-operand
	}
}

// PSW condition flags
#define CONDITION 0x0F
#define N 0x08
#define Z 0x04
#define V 0x02
#define C 0x01

void Cpu::execute(const Control *control)
{
}

void Cpu::writeBack(const Control *control)
{
	Mmu::Abort abort;

	switch(control->m_Zloc)
	{
	case OpLoc_None:
		break;

	case OpLoc_Reg:
		m_r[control->m_Zaddr] = control->m_result;
		break;

	case OpLoc_Mem:
		m_mmu->write(control->m_Zaddr, control->m_result, &abort);
		break;

	default:
		Serial.print("wb: unexpected oploc ");
		Serial.print(control->m_Zloc);
		Serial.println(" for result");
		Serial.print(dump());
		break;
	}

	m_psw = control->m_psw;
}

uint16_t Cpu::getReg(uint8_t reg)
{
	return m_r[reg];
}

String Cpu::dump() const
{
	char dump[150]; // it's gonna be a buffer overflow party tonight

	sprintf(dump, "r0: %06o r1: %06o r2: %06o r3: %06o\r\n"
				  "r4: %06o r5: %06o sp: %06o pc: %06o\r\n"
				  "psw: %06o\r\n",
		m_r[0], m_r[1], m_r[2], m_r[3], m_r[4], m_r[5], m_sp, m_pc, m_psw);

	return String(dump);
}