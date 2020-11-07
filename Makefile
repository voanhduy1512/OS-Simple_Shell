all: simple-shell.c
	gcc -Wall -O2 -o simple-shell simple-shell.c command.c -lreadline
run:
	./simple-shell
debug:
	gcc -Wall -g -o simple-shell simple-shell.c command.c -lreadline
clean:
	rm simple-shell
