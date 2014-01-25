#include <Arduino.h>

#include "cpu.h"
#include "mmu.h"

#ifndef _BIST_H
#define _BIST_H

void bist(Cpu *cpu, Mmu *mmu);

#endif /* _BIST_H */