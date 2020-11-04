#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define SHELL_PROMPT "\x1b[32m\x1b[1mosh>\x1b[0m "

char* get_current_working_directory(){
  return getcwd(NULL, 1024);
}

void print_current_working_directory(){
  char* current_working_directory = get_current_working_directory();
  
  fputs("\x1b[34m\x1b[1m", stdout);
  fputs(current_working_directory, stdout);
  fputs("\x1B[0m\n", stdout);
  
  free(current_working_directory);
}

char* get_user_command(){
  return readline(SHELL_PROMPT);
}

char* push_back(char* string, int* string_length, char new_char){
  char* result = realloc(string, (*string_length + 2) * sizeof(char));
  result[*string_length] = new_char;
  (*string_length)++;
  result[*string_length] = '\0';
  
  return result;
}

char* standardize(char* string){
  char* new_string = NULL;

  int it_string = 0;
  int it_new_string = 0;
  while (string[it_string] == ' ') it_string++;

  for (; string[it_string]; it_string++){
    if (it_new_string == 0)
      new_string = push_back(new_string, &it_new_string, string[it_string]);
    else{
      if (string[it_string - 1] == ' ' && string[it_string] == ' ') continue;
      else
	if (string[it_string - 1] == ' ' && string[it_string] != ' '){
	  new_string = push_back(new_string, &it_new_string, ' ');
	  new_string = push_back(new_string, &it_new_string, string[it_string]);
	}
	else
	  if (string[it_string - 1] != ' ' && string[it_string] != ' ')
	    new_string = push_back(new_string, &it_new_string, string[it_string]);
    }
  }
  
  return new_string;
}

int is_exit_command(char* user_command){
  return (strcmp(user_command, "exit") == 0);
}

int main(){
  int is_running = 1;
  
  while(is_running){
    print_current_working_directory();
    char* user_command = get_user_command();
    user_command = standardize(user_command);
    fputs(user_command, stdout);

    if (is_exit_command(user_command) == 1) break;
  }
}


