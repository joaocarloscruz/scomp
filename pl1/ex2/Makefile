prog: main.c
	gcc -Wall -Wextra -fanalyzer main.c -o prog

run: prog
	./prog

trace: prog
	strace -ftt -o trace.log -e trace=%process ./prog
