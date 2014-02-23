OBJS = bist.o cpu.o linux_main.o mmu.o

all: bist
clean:
	rm -f bist *.o

.PHONY: all clean

bist: $(OBJS)
	$(CXX) -o $@ $(OBJS)