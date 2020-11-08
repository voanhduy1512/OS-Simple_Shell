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

struct basic_command_list parse_command_to_basic_command_list(const char* command){
  char* command_copy = malloc((strlen(command) + 1) * sizeof(char));
  strcpy(command_copy, command);

  struct basic_command_list result = NULL_BASIC_COMMAND_LIST;
  result.arg = (char*)command_copy;

  if (command_copy[0] == '\0'){
    free(command_copy);
    free_basic_command_list(result);
    return NULL_BASIC_COMMAND_LIST;
  }

  struct basic_command basic_command = NULL_BASIC_COMMAND;

  int start_position;
  int end_position = 0;
  
  do {
    //End of last iteration = Start of new iteration.
    start_position = end_position;

    int split_part_return = split_part(command_copy, &end_position);

    //Error when split
    if (split_part_return == -1){
      fputs("Error: missing closing \".\n", stderr);
      
      free(command_copy);
      free_basic_command(basic_command);
      free_basic_command_list(result);
      return NULL_BASIC_COMMAND_LIST;
    }

    //is_arg = 1 if current split part is string (usually string will be an argument)
    //else = 0
    int is_arg = split_part_return;
    int is_not_string = !split_part_return;
    int is_done = 0;

    if (command_copy[start_position] == '\0') //End of command
      is_done = 1;
    else 
      //If not string part
      if (is_not_string){
	if (command_copy[start_position] == '|'){
	  basic_command.pipe = 1;
	  is_done = 1;
	}
	
	else  
	if (command_copy[start_position] == '&')
	  basic_command.async = 1;
	
	else
	if (command_copy[start_position] == '<')
	  basic_command.file_name[0] = get_file_name(command_copy, &end_position);
	
	else  
	if (command_copy[start_position] == '>')
	  basic_command.file_name[1] = get_file_name(command_copy, &end_position);
	
	else //If not in any special case then it is an argument.
	  is_arg = 1;
      }

    if (is_arg)
      add_arg_to_basic_command(basic_command, command_copy + start_position);

    if (is_done){
      if (basic_command.argc == 0){
	fputs("Error : Command with no argument.\n", stderr);
	
	free(command_copy);
	free_basic_command(basic_command);
	free_basic_command_list(result);
	return NULL_BASIC_COMMAND_LIST;
      }

      basic_command.argv[basic_command.argc] = NULL;

      add_basic_command_to_list(result, basic_command);
      
      basic_command = NULL_BASIC_COMMAND;
    }
  } while (command_copy[start_position] != '\0');

  free(command_copy);
  free_basic_command(basic_command);

  return result;
}
