#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basic_command.h"

void end_part_and_split_done(char* command, int* end_pos, int* is_done){
  command[(*end_pos)++] = '\0';
  *is_done = 1;
}

//Split a part or a string in command
//Split end when current char is space and not in a string
//Return 1 if split a string (between " ")
//Return -1 if error when not close string (missing ")
//Else return 0
int split_part(char* command, int* start_position){
  int is_string = 0;
  int is_in_string = 0;
  int is_done = 0;

  int i, j;
  i = j = *start_position;

  for(; command[i]; i++){
    if (is_in_string == 1){
      if (command[i] == '"')
	is_in_string = 0;
      else if (command[i] == '\\')
	command[j++] = command[++i];
      else
	command[j++] = command[i];
    }
    else if (command[i] == ' '){
      end_part_and_split_done(command, &j, &is_done);
      i++;
      break;
    }
    else if (command[i] == '"'){
      is_string = 1;
      is_in_string = 1;
    }
    else
      command[j++] = command[i];
  }

  //If reach end of command and not is in string then split done.
  if (is_in_string == 0 && !command[i])
    end_part_and_split_done(command, &j, &is_done);

  if (is_done != 1)
    return -1;

  *start_position = i;
  if (command[i] == ' ')
    (*start_position)++;

  return is_string;
}

char* get_file_name(char* command, int* start_position){
  int start_position_copy = *start_position;
  split_part(command, start_position);
  return command + start_position_copy;
}

int is_end_position_of_command(char* command, int pos){
  return (command[pos] == '\0');
}

char* copy_string(const char* source){
  char* copy_string = malloc((strlen(source) + 1) * sizeof(char));
  strcpy(copy_string, source);
  copy_string[strlen(source)] = '\0';
  return copy_string;
}

struct basic_command_list parse_command_to_basic_command_list(const char* command){
  char* command_copy = copy_string(command);
  struct basic_command_list result = NULL_BASIC_COMMAND_LIST;
  struct basic_command basic_command = NULL_BASIC_COMMAND;

  if (command_copy[0] == '\0')
    goto error_free_all_and_return_null;

  int start_position;
  int end_position = 0;
  
  do {
    //End of last iteration = Start of new iteration.
    start_position = end_position;

    int split_part_return = split_part(command_copy, &end_position);

    //Error when split
    if (split_part_return == -1){
      fputs("Error: missing closing \".\n", stderr);
      goto error_free_all_and_return_null;
    }

    //is_arg = 1 if current split part is string (usually string will be an argument)
    //else = 0
    int is_arg = split_part_return;
    int is_not_string = !split_part_return;
    int is_done = 0;

    if (is_end_position_of_command(command_copy, start_position))
      is_done = 1;
    else 
      if (is_not_string){
	if (command_copy[start_position] == '|'){
	  basic_command.pipe = 1;
	  is_done = 1;
	}
	
	else if (command_copy[start_position] == '&')
	  basic_command.async = 1;
	
	else if (command_copy[start_position] == '<')
	  basic_command.file_name[0] = get_file_name(command_copy, &end_position);
	
	else if (command_copy[start_position] == '>')
	  basic_command.file_name[1] = get_file_name(command_copy, &end_position);

	//If not in any special case then it is an argument.
	else is_arg = 1;
      }

    if (is_arg)
      add_arg_to_basic_command(&basic_command, command_copy + start_position);

    if (is_done){
      if (basic_command.argc == 0){
	fputs("Error : Command with no argument.\n", stderr);
	goto error_free_all_and_return_null;
      }

      basic_command.argv[basic_command.argc] = NULL;
      add_basic_command_to_list(&result, basic_command);
      basic_command = NULL_BASIC_COMMAND;
    }
  } while (!is_end_position_of_command(command_copy, start_position));

  free(command_copy);
  free_basic_command(&basic_command);

  return result;

 error_free_all_and_return_null:
  free(command_copy);
  free_basic_command(&basic_command);
  free_basic_command_list(&result);
  return NULL_BASIC_COMMAND_LIST;
}
