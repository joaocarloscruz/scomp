
simulator: main.c
	gcc -Wall -Wextra -fanalyzer main.c -o simulator

drone_program: drone.c
	gcc -Wall -Wextra -fanalyzer drone.c -o drone_program

run: simulator drone_program
	./simulator

trace: simulator drone_program
	strace -f -tt -o trace.log -e trace=%process ./simulator

clean:
	rm -f simulator drone_program trace.log *.o core.*

