#include <stdio.h>
#include <stdlib.h>

#include "basic_command.h"

void free_basic_command(struct basic_command basic_command){
  free(basic_command.argv);
  free(basic_command.file_name[0]);
  free(basic_command.file_name[1]);
}

void free_basic_command_list(struct basic_command_list basic_commands){
  for (int i = 0; i < basic_commands.command_count; i++)
    free_basic_command(basic_commands.basic_command[i]);

  free(basic_commands.basic_command);
  free(basic_commands.arg);
}

void add_arg_to_basic_command(struct basic_command basic_command, char* arg){
  basic_command.argc++;
  basic_command.argv = realloc(basic_command.argv, (basic_command.argc + 1) * sizeof(char*));
  basic_command.argv[basic_command.argc - 1] = arg;
}
