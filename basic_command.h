#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#ifndef NULL
#define NULL (void *)0
#endif

struct basic_command{
  int argc;
  char** argv;
  int async;

  char* file_name[2];
  int pipe;
};

static const struct basic_command NULL_BASIC_COMMAND = {0, NULL, 0, {NULL, NULL}, 0};

struct basic_command_list{
  int command_count;
  struct basic_command* basic_command;
};

static const struct basic_command_list NULL_BASIC_COMMAND_LIST = {0, NULL};

void free_basic_command(struct basic_command* basic_command);
void free_basic_command_list(struct basic_command_list* basic_commands);

int execute_basic_command_list(struct basic_command_list basic_commands);

struct basic_command_list parse_command_to_basic_command_list(const char* command);

void add_arg_to_basic_command(struct basic_command* basic_command, char* arg);

void add_basic_command_to_list(struct basic_command_list* basic_commands, struct basic_command basic_command);

#endif //COMMAND_HEADER
