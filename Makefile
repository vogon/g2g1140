OBJS = bist.o cpu.o main.o mmu.o

all: bist
.PHONY: all

bist: $(OBJS)
	$(CXX) -o $@ $(OBJS)