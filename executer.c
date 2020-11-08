#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#include "basic_command.h"

int cd(char **argv) {
  int res = 0;
  if (!argv[1])
    res = chdir(getenv("HOME"));
  else if (argv[2])
    fputs("Error: Too many arguments.\n", stderr);
  else{
    if (argv[1][0] == '~'){
      res = chdir(getenv("HOME"));
      argv[1][0] = '.';
    }
        
    if (res >= 0)
      res = chdir(argv[1]);
  }

  return res;
}

pid_t execute_basic_command(char **argv, const int fd[2])
{
  if (strcmp(argv[0], "cd") == 0) {
    cd(argv);
    return -1;
  }

  pid_t process_id = vfork();
  if (process_id < 0) {
    perror("Execute fork failed.\n");
    return -1;
  }

  if (process_id == 0) {
    if (fd[0] >= 0) {
      if (dup2(fd[0], STDIN_FILENO) < 0) {
	perror("Execute input redirect failed.\n");
	exit(1);
      }
      close(fd[0]);
    }

    if (fd[1] >= 0) {
      if (dup2(fd[1], STDOUT_FILENO) < 0) {
	perror("Execute output redirect failed.\n");
	exit(1);     
      }
      close(fd[1]);
    }
    
    execvp(argv[0], argv);
    perror("Execute failed.\n");
    exit(1);
  }
  else {
    if (fd[0] >= 0)
      close(fd[0]);
        
    if (fd[1] >= 0)
      close(fd[1]);

    return process_id;
  }
}

int redir_in(int fd[2], char *file_name) {
  if (fd[0] >= 0) {
    fputs("Error: Have more than one input.\n", stderr);
    return -1;
  }

  fd[0] = open(file_name, O_RDONLY, 0);
  
  if (fd[0] < 0) {
    perror("Error : Failed to open file to read.\n");
    return -1;
  }

  return 0;
}

int redir_out(int fd[2], char *file_name) {
  if (fd[1] >= 0) {
    fputs("Error: Have more than one input.\n", stderr);
    return -1;
  }

  const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  fd[1] = open(file_name, O_WRONLY | O_CREAT, mode);
  
  if (fd[1] < 0){
    perror("Error : Failed to open file to write.\n");
    return -1;
  }
  
  return 0;
}

int pipe_next(int current_fd[2], int next_fd[2]) {
  if (current_fd[1] >= 0 || next_fd[0] >= 0) {
    fputs("Error: Have more than one input.\n", stderr);
    return -1;
  }

  int pfd[2];
  if (pipe(pfd) < 0) {
    perror("Error : Pipe failed.\n");
    return -1;
  }

  current_fd[1] = pfd[1];
  next_fd[0] = pfd[0]; 
  return 0;
}

void move_fd(int current_fd[2], int next_fd[2]) {
  current_fd[0] = next_fd[0];
  current_fd[1] = next_fd[1];
  next_fd[0] = next_fd[1] = -1;
}

int execute_basic_command_list(struct basic_command_list basic_commands) {
  int current_fd[2] = {-1, -1};
  int next_fd[2] = {-1, -1};

  pid_t *process_id = malloc(basic_commands.command_count * sizeof(pid_t));

  int i;
  for (i = 0; i < basic_commands.command_count; ++i) {
    struct basic_command *basic_command = basic_commands.basic_command + i;
    
    if (basic_command->file_name[0] && redir_in(current_fd, basic_command->file_name[0]) < 0)
      return -1;

    if (basic_command->file_name[1] && redir_out(current_fd, basic_command->file_name[1]) < 0)
      return -1;

    if (basic_command->pipe && pipe_next(current_fd, next_fd) < 0)
      return -1;

    process_id[i] = execute_basic_command(basic_command->argv, current_fd);

    if (!basic_command->pipe && !basic_command->async && process_id[i] >= 0)
      waitpid(process_id[i], NULL, 0);

    move_fd(current_fd, next_fd);
  }

  for (i = 0; i < basic_commands.command_count; ++i)
    if (basic_commands.basic_command[i].pipe && !basic_commands.basic_command[i].async && process_id[i] >= 0)
      waitpid(process_id[i], NULL, 0);

  free(process_id);

  return 0;
}
