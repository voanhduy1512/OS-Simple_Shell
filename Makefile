all: shell.c
	gcc -Wall -O2 -o shell shell.c -lreadline
run:
	./shell
clean:
	rm shell
