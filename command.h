#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#ifndef NULL
#define NULL (void *)0
#endif

struct command{
  int argc;
  char** argv;
  int async;

  char* file_name[2];
  int pipe;
};

static const struct command NULL_COMMAND = {0, NULL, 0, {NULL, NULL}, 0};

struct command_list{
  int command_count;
  struct command *commands;
  char* arg;
};

static const struct command_list NULL_COMMAND_LIST = {0, NULL, NULL};

void free_command(struct command_list commands);
int execute_command(struct command_list commands);
struct command_list parse_commands(const char* user_command);

#endif //COMMAND_HEADER
