prog: main.c
	gcc -Wall -Wextra -fanalyzer -g main.c -o prog -lpthread

ts: main.c
	gcc -Wall -Wextra -fanalyzer -fsanitize=thread -g main.c -o ts -lpthread

run: prog
	./prog

vg: prog
	valgrind --tool=helgrind ./prog

drd: prog
	valgrind --tool=drd ./prog

clean:
	rm -fv prog ts
