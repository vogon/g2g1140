#include "mmu.h"

#define PHYS_SIZE 0777777

Mmu::Mmu(size_t ramSize)
{
	m_ram = new uint8_t[ramSize];
	m_ramSize = ramSize;
	m_mmuEnable = false;
}

Mmu::~Mmu()
{
	delete[] m_ram;
}

void Mmu::setEnabled(bool enabled)
{
	m_mmuEnable = enabled;
}

uint16_t Mmu::read(uint16_t virt, Abort *abort) const
{

}

void Mmu::write(uint16_t virt, uint16_t value, Abort *abort)
{

}

uint32_t Mmu::virt2phys(uint16_t virt, Abort *abort)
{
	if (m_mmuEnable)
	{
		// virtual addressing logic, per hbk ss. 6.2
		abort->aborted = true; // NYI
	}
	else
	{
		// basic addressing logic, per hbk ss. 6.1
		if (virt & 0170000 == 0170000)
		{
			return 0770000 | virt;
		}
		else
		{
			return virt;
		}
	}
}