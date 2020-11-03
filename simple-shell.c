#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define SHELL_PROMPT "\x1b[32m\x1b[1mosh>\x1b[0m "

int is_running = 0;

void handler(int signal){
  fputs("\n", stdout);

  if(!is_running)
    fputs(SHELL_PROMPT, stdout);
}

int main(){
  signal(SIGINT, handler);

  
}



