

all: unixsh sudoku monte_carlo

unixsh: unixsh.c
	gcc -o unixsh unixsh.c -Wall

sudoku: sudoku.c
	gcc -o sudoku sudoku.c -lpthread -Wall

monte_carlo: monte_carlo.c
	gcc -o monte_carlo monte_carlo.c -Wall

clean:
	rm -f unixsh sudoku monte_carlo