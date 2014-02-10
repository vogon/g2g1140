#include <Arduino.h>

#include "mmu.h"

#ifndef _CPU_H
#define _CPU_H

struct cpu;
typedef struct cpu cpu_t;

cpu_t *cpu_create(mmu_t *mmu);
void cpu_destroy(cpu_t *cpu);

void cpu_reset(cpu_t *cpu);

uint16_t cpu_peek_reg(cpu_t *cpu, uint8_t reg);
void cpu_poke_reg(cpu_t *cpu, uint8_t reg, uint16_t value);

void cpu_step(cpu_t *cpu);

String cpu_dump(cpu_t *cpu);

#endif /* _CPU_H */