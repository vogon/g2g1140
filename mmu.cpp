#include <memory.h>
#include <stdio.h>

#include "mmu.h"

const uint32_t N_OVERLAYS = 64;
const uint32_t PHYS_SIZE = 01000000;

typedef struct _mmu_overlay
{
	uint32_t base;
	uint32_t size;

	overlay_read_byte_t readByte;
	overlay_read_word_t readWord;
	overlay_write_byte_t writeByte;
	overlay_write_word_t writeWord;
}
mmu_overlay_t;

struct _mmu
{
	bool enable;
	uint16_t *ram;
	size_t ramSize;

	mmu_overlay_t *overlays[N_OVERLAYS];
	uint32_t nextOverlayIndex;
};

uint32_t _mmu_virt2phys(mmu_t *mmu, uint32_t virt);
mmu_overlay_t *_mmu_find_overlay(mmu_t *mmu, uint32_t phys);
void _mmu_install_core_overlay(mmu_t *mmu, size_t ramSize);

mmu_t *mmu_create(size_t ramSize)
{
	if (ramSize % 2 != 0) return NULL;

	mmu_t *mmu = (mmu_t *)malloc(sizeof(mmu_t));

	mmu->enable = false;
	mmu->ram = (uint16_t *)calloc(ramSize, 1);
	mmu->ramSize = ramSize;

	memset(mmu->overlays, N_OVERLAYS, sizeof(mmu_overlay_t *));
	mmu->nextOverlayIndex = 0;

	_mmu_install_core_overlay(mmu, ramSize);

	return mmu;
}

void mmu_destroy(mmu_t *mmu)
{
	for (int i = 0; i < N_OVERLAYS; i++)
	{
		free(mmu->overlays[i]);
	}

	free(mmu->ram);
	free(mmu);
}

void mmu_set_enabled(mmu_t *mmu, bool enabled)
{
	mmu->enable = enabled;
}

void mmu_add_overlay(mmu_t *mmu, uint32_t basePhys, uint32_t size, 
	overlay_read_byte_t readByte, overlay_read_word_t readWord,
	overlay_write_byte_t writeByte, overlay_write_word_t writeWord)
{
	if (mmu->nextOverlayIndex >= N_OVERLAYS)
	{
#ifdef ARDUINO
		Serial.print("mmu_add_overlay: too many overlays, new overlay ignored");
#else
		printf("mmu_add_overlay: too many overlays, new overlay ignored\n");
#endif
	}
	else
	{
		mmu_overlay_t *overlay = (mmu_overlay_t *)malloc(sizeof(mmu_overlay_t));

		overlay->base = basePhys;
		overlay->size = size;
		overlay->readByte = readByte;
		overlay->readWord = readWord;
		overlay->writeByte = writeByte;
		overlay->writeWord = writeWord;

		mmu->overlays[mmu->nextOverlayIndex] = overlay;
		mmu->nextOverlayIndex++;
	}
}

uint32_t _mmu_virt2phys(mmu_t *mmu, uint32_t virt)
{
	if (mmu->enable)
	{
		// TODO: page tables and stuff
	}
	else
	{
		// hard-wired flat map, per processor handbook ss. 6.1
		if (virt >= 0170000)
		{
			return 0770000 | virt;
		}
		else
		{
			return virt;
		}
	}
}

mmu_overlay_t *_mmu_find_overlay(mmu_t *mmu, uint32_t phys)
{
	if (phys >= PHYS_SIZE) return NULL;

	for (int i = 0; i < N_OVERLAYS; i++)
	{
		// assume all null overlays are at the end
		if (mmu->overlays[i] == NULL) return NULL;

		mmu_overlay_t *ovl = mmu->overlays[i];

		if ((ovl->base) <= phys && (ovl->base + ovl->size) > phys)
		{
			// phys is contained in this overlay
			return ovl;
		}
	}
}

uint8_t mmu_read_byte(mmu_t *mmu, uint16_t virt, mmu_abort_t *abort)
{
	uint32_t phys = _mmu_virt2phys(mmu, virt);
	mmu_overlay_t *overlay = _mmu_find_overlay(mmu, phys);

	if (overlay != NULL)
	{
		// address is mapped, compute offset and fetch byte
		uint32_t offset = virt - overlay->base;

		return overlay->readByte(mmu, offset, abort);
	}
	else
	{
		// address isn't mapped; time-out-error abort (pp. A-8)
		abort->aborted = abort->timeOutError = true;
	}
}

uint16_t mmu_read_word(mmu_t *mmu, uint16_t virt, mmu_abort_t *abort)
{
	uint32_t phys = _mmu_virt2phys(mmu, virt);
	mmu_overlay_t *overlay = _mmu_find_overlay(mmu, phys);

	if (overlay != NULL)
	{
		// address is mapped, compute offset and fetch byte
		uint32_t offset = virt - overlay->base;

		return overlay->readWord(mmu, offset, abort);
	}
	else
	{
		// address isn't mapped; time-out-error abort (pp. A-8)
		abort->aborted = abort->timeOutError = true;
	}
}

void mmu_write_byte(mmu_t *mmu, uint16_t virt, uint8_t value,
	mmu_abort_t *abort)
{
	uint32_t phys = _mmu_virt2phys(mmu, virt);
	mmu_overlay_t *overlay = _mmu_find_overlay(mmu, phys);

	if (overlay != NULL)
	{
		// address is mapped, compute offset and fetch byte
		uint32_t offset = virt - overlay->base;
		overlay->writeByte(mmu, offset, value, abort);
	}
	else
	{
		// address isn't mapped; time-out-error abort (pp. A-8)
		abort->aborted = abort->timeOutError = true;
	}
}

void mmu_write_word(mmu_t *mmu, uint16_t virt, uint16_t value,
	mmu_abort_t *abort)
{
	uint32_t phys = _mmu_virt2phys(mmu, virt);
	mmu_overlay_t *overlay = _mmu_find_overlay(mmu, phys);

	if (overlay != NULL)
	{
		// address is mapped, compute offset and fetch byte
		uint32_t offset = virt - overlay->base;
		overlay->writeWord(mmu, offset, value, abort);
	}
	else
	{
		// address isn't mapped; time-out-error abort (pp. A-8)
		abort->aborted = abort->timeOutError = true;
	}
}

uint8_t _mmu_core_read_byte(mmu_t *mmu, uint32_t offset, mmu_abort_t *abort)
{
	uint32_t wordOffset = offset >> 1;

	if (offset & 0x1 == 0)
	{
		return (uint8_t)(mmu->ram[wordOffset] & 0xff);
	}
	else
	{
		return (uint8_t)((mmu->ram[wordOffset] >> 8) & 0xff);
	}
}

uint16_t _mmu_core_read_word(mmu_t *mmu, uint32_t offset, mmu_abort_t *abort)
{
	if (offset & 0x1 != 0)
	{
		// address is unaligned, abort
		abort->aborted = abort->boundaryError = true;
		return 0;
	}

	uint32_t wordOffset = offset >> 1;
	return mmu->ram[wordOffset];
}

void _mmu_core_write_byte(mmu_t *mmu, uint32_t offset, uint8_t value, 
	mmu_abort_t *abort)
{
	uint32_t wordOffset = offset >> 1;

	if (offset & 0x1 == 0)
	{
		uint16_t unchanged = mmu->ram[wordOffset] & 0xff00;
		mmu->ram[wordOffset] = unchanged | value;
	}
	else
	{
		uint16_t unchanged = mmu->ram[wordOffset] & 0x00ff;
		mmu->ram[wordOffset] = unchanged | (value << 8);
	}
}

void _mmu_core_write_word(mmu_t *mmu, uint32_t offset, uint16_t value, 
	mmu_abort_t *abort)
{
	if (offset & 0x1 != 0)
	{
		// address is unaligned, abort
		abort->aborted = abort->boundaryError = true;
		return;
	}

	uint32_t wordOffset = offset >> 1;
	mmu->ram[wordOffset] = value;
}

void _mmu_install_core_overlay(mmu_t *mmu, size_t ramSize)
{
	mmu_add_overlay(mmu, 0, ramSize, 
		_mmu_core_read_byte, _mmu_core_read_word,
		_mmu_core_write_byte, _mmu_core_write_word);
}