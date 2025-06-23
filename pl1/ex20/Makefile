all: guess cheat

guess: guess.c
	gcc guess.c -o guess

cheat: cheat.c
	gcc cheat.c -o cheat

run: guess
	./guess

clean:
	rm -f guess cheat