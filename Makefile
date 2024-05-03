.PHONY: run

all: clean compile

compile:
	gcc compiler.c -o compiler.out
	./compiler.out
	nasm -felf64 assembly.asm -o machine.o
	ld machine.o -o machine

clean:
	rm -f compiler.out stage1 assembly.asm machine.o machine