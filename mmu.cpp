#include "mmu.h"

const uint32_t N_OVERLAYS = 64;
const uint32_t PHYS_SIZE = 0777777;

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
	uint8_t *ram;
	size_t ramSize;

	mmu_overlay_t *overlays[N_OVERLAYS];
	uint32_t nextOverlayIndex;
};


mmu_t *mmu_create(size_t ramSize)
{
	mmu_t *mmu = (mmu_t *)malloc(sizeof(mmu_t));

	mmu->enable = false;
	mmu->ram = (uint8_t *)calloc(ramSize, 1);
	mmu->ramSize = ramSize;

	memset(mmu->overlays, N_OVERLAYS, sizeof(mmu_overlay_t *));

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
		Serial.print("mmu_add_overlay: too many overlays, new overlay ignored");
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

void mmu_read_byte(mmu_t *mmu, uint16_t virt, mmu_abort_t *abort)
{
	
}