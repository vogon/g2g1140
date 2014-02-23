#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdlib.h>
#include <stdint.h>
#endif

#ifndef _MMU_H
#define _MMU_H

#define CHECK_ABORT(a) if ((a)->aborted) goto mmu_abort;

struct _mmu;
typedef struct _mmu mmu_t;

typedef struct _mmu_abort 
{
	_mmu_abort() : aborted(false), 
		boundaryError(false), timeOutError(false) {}

	bool aborted;
	bool boundaryError;
	bool timeOutError;
} 
mmu_abort_t;

typedef uint8_t (*overlay_read_byte_t)(mmu_t *,
	uint32_t /*offset*/, mmu_abort_t *);
typedef uint16_t (*overlay_read_word_t)(mmu_t *,
	uint32_t /*offset*/, mmu_abort_t *);

typedef void (*overlay_write_byte_t)(mmu_t *,
	uint32_t /*offset*/, uint8_t /*value*/, mmu_abort_t *);
typedef void (*overlay_write_word_t)(mmu_t *,
	uint32_t /*offset*/, uint16_t /*value*/, mmu_abort_t *);

mmu_t *mmu_create(size_t ramSize);
void mmu_destroy(mmu_t *mmu);

void mmu_set_enabled(mmu_t *mmu, bool enabled);
void mmu_add_overlay(mmu_t *mmu, uint32_t basePhys, uint32_t size, 
	overlay_read_byte_t readByte, overlay_read_word_t readWord,
	overlay_write_byte_t writeByte, overlay_write_word_t writeWord);

uint8_t mmu_read_byte(mmu_t *mmu, uint16_t virt, mmu_abort_t *abort);
uint16_t mmu_read_word(mmu_t *mmu, uint16_t virt, mmu_abort_t *abort);
void mmu_write_byte(mmu_t *mmu, uint16_t virt, uint8_t value,
	mmu_abort_t *abort);
void mmu_write_word(mmu_t *mmu, uint16_t virt, uint16_t value,
	mmu_abort_t *abort);

#endif /* _MMU_H */