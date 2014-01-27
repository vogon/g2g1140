#include "mmu.h"

#define PHYS_SIZE 0777777

Mmu::Mmu(size_t ramSize)
{
	m_mmuEnable = false;

	// overlay management data
	memset(&m_overlays, 0, sizeof(m_overlays));
	m_nextOverlay = 0;

	// set up the RAM "overlay"
	m_ram = new uint8_t[ramSize];
	m_ramSize = ramSize;
	addOverlay(0, ramSize, &_readRam, &_writeRam, this);
}

Mmu::~Mmu()
{
	for (int i = 0; i < N_OVERLAYS; i++)
	{
		delete m_overlays[i];
	}

	delete[] m_ram;
}

void Mmu::setEnabled(bool enabled)
{
	m_mmuEnable = enabled;
}

void Mmu::addOverlay(uint32_t basePhys, uint32_t size, overlay_read_t read,
	overlay_write_t write, void *userData)
{
	Overlay *newOverlay = new Overlay();

	newOverlay->base = basePhys;
	newOverlay->size = size;
	newOverlay->read = read;
	newOverlay->write = write;
	newOverlay->userData = userData;

	if (m_nextOverlay >= N_OVERLAYS)
	{
		Serial.println("mmu: warning: out of overlays; new overlay ignored");
	}
	else
	{
		m_overlays[m_nextOverlay++] = newOverlay;
	}
}

uint16_t Mmu::read(uint16_t virt, Abort *abort) const
{
	uint32_t phys = virt2phys(virt, abort);
	CHECK_ABORT(abort);

	for (int i = 0; i < m_nextOverlay; i++)
	{
		Overlay *overlay = m_overlays[i];

		Serial.print("read: ");
		Serial.print(phys, OCT);
		Serial.print(" <>? ");
		Serial.print(overlay->base, OCT);
		Serial.print("...");
		Serial.println((overlay->base + overlay->size), OCT);

		if ((phys >= overlay->base) && 
			(phys < (overlay->base + overlay->size)))
		{
			// address is within this overlay's region
			return overlay->read(overlay->userData, 
				phys - overlay->base, abort);
		}
	}

	// address wasn't mapped
	// TODO: write abort code
mmu_abort:
	return 0;
}

void Mmu::write(uint16_t virt, uint16_t value, Abort *abort)
{
	uint32_t phys = virt2phys(virt, abort);
	CHECK_ABORT(abort);

	for (int i = 0; i < m_nextOverlay; i++)
	{
		Overlay *overlay = m_overlays[i];

		Serial.print("write: ");
		Serial.print(phys, OCT);
		Serial.print(" <>? ");
		Serial.print(overlay->base, OCT);
		Serial.print("...");
		Serial.println((overlay->base + overlay->size), OCT);

		if ((phys >= overlay->base) &&
			(phys < (overlay->base + overlay->size)))
		{
			// address is within this overlay's region
			overlay->write(overlay->userData, phys - overlay->base,
				value, abort);
			return;
		}
	}

	// address wasn't mapped
	// TODO: write abort code
mmu_abort:
	return;
}

uint32_t Mmu::virt2phys(uint16_t virt, Abort *abort) const
{
	if (m_mmuEnable)
	{
		// virtual addressing logic, per hbk ss. 6.2
		abort->aborted = true; // NYI
		return 0;
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

uint16_t Mmu::readRam(uint32_t offset, Abort *abort)
{
	Serial.print("readRam(");
	Serial.print(offset, OCT);
	Serial.println(")");

	if (offset >= m_ramSize)
	{
		// todo: MMU abort
		abort->aborted = true;
		return 0;
	}
	else
	{
		return m_ram[offset];
	}
}

void Mmu::writeRam(uint32_t offset, uint16_t value, Abort *abort)
{
	Serial.print("writeRam(");
	Serial.print(offset, OCT);
	Serial.print(", ");
	Serial.print(value, OCT);
	Serial.println(")");

	if (offset >= m_ramSize)
	{
		// todo: MMU abort
		abort->aborted = true;
	}
	else
	{
		m_ram[offset] = value;
	}
}