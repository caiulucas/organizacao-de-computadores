all: main.o cpu.o generator.o instruction.o file.o memory.o mmu.o
	@gcc main.o cpu.o generator.o instruction.o file.o memory.o mmu.o -o exe  -lm
	@rm -r *.o
main.o: main.c
	@gcc main.c -c -Wall
cpu.o: cpu.c
	@gcc cpu.c -c -Wall
generator.o: generator.c
	@gcc generator.c -c -Wall
instruction.o: instruction.c
	@gcc instruction.c -c -Wall
file.o: file.c
	@gcc file.c -c -Wall
memory.o: memory.c
	@gcc memory.c -c -Wall
mmu.o: mmu.c
	@gcc mmu.c -c -Wall
run:
	@./exe
