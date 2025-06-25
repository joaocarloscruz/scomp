prog: main_prog factorial

main_prog: main.c
	gcc main.c -o main_prog

factorial: factorial.c
	gcc factorial.c -o factorial

run: prog
	./main_prog

clean:
	rm -f main_prog factorial
