#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#include "command.h"

//SHELL_PROMPT = "osh> " with green and bold text and a normal white space
#define SHELL_PROMPT "\x1b[32m\x1b[1mosh>\x1b[0m "

char* get_current_working_directory(){
  return getcwd(NULL, 1024);
}

void print_current_working_directory(){
  char* current_working_directory = get_current_working_directory();

  //Print current working directory with blue and bold text
  fputs("\x1b[36m\x1b[1m", stdout);
  fputs(current_working_directory, stdout);
  fputs("\x1b[0m\n", stdout);
  
  free(current_working_directory);
}

char* get_user_command(){
  //Print SHELL_PROMPT then readline.
  return readline(SHELL_PROMPT);
}

char* push_back(char* string, int* string_length, char new_char){
  char* result = realloc(string, (*string_length + 2) * sizeof(char));
  result[*string_length] = new_char;
  (*string_length)++;
  result[*string_length] = '\0';
  
  return result;
}

int get_first_index_which_not_space(char* string){
  int index = 0;
  while (string[index] == ' ') index++;
  return index;
}

char* remove_useless_space(char* string){
  char* new_string = NULL;

  int it_string = get_first_index_which_not_space(string);
  int current_new_string_length = 0;

  for (; string[it_string]; it_string++){
    if (current_new_string_length == 0)
      new_string = push_back(new_string, &current_new_string_length, string[it_string]);
    else{
      //Skip when "<space><space>".
      if (string[it_string - 1] == ' ' && string[it_string] == ' ')
	continue;

      //When it is "<space><not space>", add space.
      if (string[it_string - 1] == ' ' && string[it_string] != ' ')
	new_string = push_back(new_string, &current_new_string_length, ' ');

      //When current char is not string, add it.
      if (string[it_string] != ' ')
	new_string = push_back(new_string, &current_new_string_length, string[it_string]);
    }
  }
  
  return new_string;
}

int is_exit_command(char* user_command){
  return (strcmp(user_command, "exit") == 0);
}

int is_execute_previous_command_command(char* user_command){
  return (strcmp(user_command, "!!") == 0);
}

char* get_previous_command(){
  if (history_length == 0)
    return NULL;
  else
    return (char*)history_get(history_length)->line;
}

void error_handle_empty_history(){
  fputs("No commands in history.\n", stdout);
}

void respond_about_previous_command(char* previous_command){
  if (previous_command == NULL)
    error_handle_empty_history();
  else{
    fputs(previous_command, stdout);
    fputs("\n", stdout);
  }
}

void parse_and_execute_command(char* command){
  struct basic_command_list basic_commands = parse_basic_command_list(command);

  if (basic_commands.command_count > 0){
    execute_basic_command_list(basic_commands);
    free_basic_command_list(basic_commands);
  }
}

int main(){
  int is_running = 1;
  
  while(is_running){
    print_current_working_directory();
    char* user_command = get_user_command();
    user_command = remove_useless_space(user_command);

    if (is_exit_command(user_command) == 1) break;
    
    if (is_execute_previous_command_command(user_command) == 1){
      char* previous_command = get_previous_command();
      respond_about_previous_command(previous_command);
      
      if (previous_command == NULL){
	free(user_command);
	continue;
      }
      else
	user_command = previous_command;
    }

    add_history(user_command);
    
    parse_and_execute_command(user_command);
    
    free(user_command);
  }

  return 0;
}

