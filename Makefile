all: simple-shell.c
	gcc -Wall -O2 -o simple-shell simple-shell.c basic_command.c parser.c executer.c -lreadline
run:
	./simple-shell
debug:
	gcc -Wall -g -o simple-shell simple-shell.c basic_command.c parser.c executer.c -lreadline
clean:
	rm simple-shell
