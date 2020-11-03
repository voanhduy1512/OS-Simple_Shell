all: simple-shell.c
	gcc -Wall -O2 -o simple-shell simple-shell.c -lreadline
run:
	./simple-shell
clean:
	rm simple-shell
