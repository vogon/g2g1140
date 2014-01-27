#include <Arduino.h>

#ifndef _MMU_H
#define _MMU_H

#define CHECK_ABORT(a) if ((a)->aborted) goto mmu_abort;

class Mmu
{
public:
	Mmu(size_t ramSize);
	~Mmu();

	void setEnabled(bool enabled);

	struct Abort
	{
		bool aborted;
	};

	typedef uint16_t (*overlay_read_t)(void * /*userData*/, 
		uint32_t /*offset*/, Abort *);
	typedef void (*overlay_write_t)(void * /*userData*/, 
		uint32_t /*offset*/, uint16_t /*value*/, Abort *);

	void addOverlay(uint32_t basePhys, uint32_t size, overlay_read_t read, 
		overlay_write_t write, void *userData);

	uint16_t read(uint16_t virt, Abort *abort) const;
	void write(uint16_t virt, uint16_t value, Abort *abort);

private:
	struct Overlay
	{
		uint32_t base;
		uint32_t size;
		overlay_read_t read;
		overlay_write_t write;
		void *userData;
	};

	uint32_t virt2phys(uint16_t virt, Abort *abort) const;

	static uint16_t _readRam(void *userData, uint32_t offset, Abort *abort)
	{
		Mmu *mmu = (Mmu *)userData;
		return mmu->readRam(offset, abort);
	}

	static void _writeRam(void *userData, uint32_t offset, uint16_t value,
		Abort *abort)
	{
		Mmu *mmu = (Mmu *)userData;
		mmu->writeRam(offset, value, abort);
	}

	uint16_t readRam(uint32_t offset, Abort *abort);
	void writeRam(uint32_t offset, uint16_t value, Abort *abort);

	bool m_mmuEnable;
	uint8_t *m_ram;
	size_t m_ramSize;

	static const uint32_t N_OVERLAYS = 64;
	Overlay *m_overlays[N_OVERLAYS];
	uint32_t m_nextOverlay;
};

#endif /* _MMU_H */