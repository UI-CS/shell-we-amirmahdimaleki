# CC = gcc
# CFLAGS = -Wall -g -pthread

# all: unixsh sudoku_validator monte_carlo

# unixsh: unixsh.c
# 	$(CC) $(CFLAGS) -o unixsh unixsh.c

# sudoku_validator: sudoku.c
# 	$(CC) $(CFLAGS) -o sudoku_validator sudoku.c

# monte_carlo: monte_carlo.c
# 	$(CC) $(CFLAGS) -o monte_carlo monte_carlo.c

# clean:
# 	rm -f unixsh sudoku_validator monte_carlo

all: unixsh sudoku monte_carlo

unixsh: unixsh.c
	gcc -o unixsh unixsh.c -Wall

sudoku: sudoku.c
	gcc -o sudoku sudoku.c -lpthread -Wall

monte_carlo: monte_carlo.c
	gcc -o monte_carlo monte_carlo.c -Wall

clean:
	rm -f unixsh sudoku monte_carlo