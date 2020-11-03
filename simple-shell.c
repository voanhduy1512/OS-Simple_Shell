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

char* get_current_working_directory(){
  return getcwd(NULL, 1024);
}

void print_current_working_directory(){
  char* current_working_directory = get_current_working_directory();
  
  fputs("\x1b[34m", stdout);
  fputs(current_working_directory, stdout);
  fputs("\x1B[0m\n", stdout);
  
  free(current_working_directory);
}

int main(){
  signal(SIGINT, handler);

  while(1){
    print_current_working_directory();
  }
}



