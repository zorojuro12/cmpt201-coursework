#define _POSIX_C_SOURCE 200809L
#include "../include/task0.h"
#include "../include/msgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define INPUT_SIZE 1024
#define ARG_MAX 128

// get and print the current directory for prompt
void print_prompt(void) {
  char current_dir[INPUT_SIZE];

  if (getcwd(current_dir, INPUT_SIZE) == NULL) {
    write(STDERR_FILENO, FORMAT_MSG("Shell", GETCWD_ERROR_MSG),
          strlen(FORMAT_MSG("shell", GETCWD_ERROR_MSG)));
    write(STDOUT_FILENO, "$ ", 2);
    return;
  }
  write(STDOUT_FILENO, current_dir, strlen(current_dir));
  write(STDOUT_FILENO, "$ ", 2);
}

// read one line of input
int read_line(char *buffer, int max_size) {
  ssize_t bytes = read(STDIN_FILENO, buffer, max_size - 1);

  if (bytes < 0) {
    write(STDERR_FILENO, FORMAT_MSG("shell", READ_ERROR_MSG),
          strlen(FORMAT_MSG("shell", READ_ERROR_MSG)));
    return -1;
  }

  if (bytes == 0) {
    return 0;
  }

  buffer[bytes] = '\0';

  if (bytes > 0 && buffer[bytes - 1] == '\n') {
    buffer[bytes - 1] = '\0';
  }
  return 1;
}

// splits the argument into an array
// return the number of arguments, sets is_bg if found
int split_args(char *line, char **args, int *is_bg) {
  int count = 0;
  *is_bg = 0;

  if (line[0] == '\0') {
    return 0;
  }

  // tokenize by spaces and tabs
  char *saveptr;
  char *token = strtok_r(line, " \t", &saveptr);

  while (token != NULL && count < ARG_MAX - 1) {
    args[count] = token;
    count++;
    token = strtok_r(NULL, " \t", &saveptr);
  }

  args[count] = NULL;

  if (count > 0) {
    if (strcmp(args[count - 1], "&") == 0) {
      *is_bg = 1;
      args[count - 1] = NULL;
    }
  }
  return count;
}

// execute a single command
void execute(char **args, int run_background) {
  if (args[0] == NULL) {
    return;
  }

  pid_t child_pid = fork();

  // check for fork error
  if (child_pid < 0) {
    write(STDERR_FILENO, FORMAT_MSG("shell", FORK_ERROR_MSG),
          strlen(FORMAT_MSG("shell", FORK_ERROR_MSG)));
    return;
  }

  if (child_pid == 0) {
    execvp(args[0], args);

    write(STDERR_FILENO, FORMAT_MSG("shell", EXEC_ERROR_MSG),
          strlen(FORMAT_MSG("shell", EXEC_ERROR_MSG)));
    _exit(EXIT_FAILURE);
  }

  if (run_background == 0) {
    int status;
    if (waitpid(child_pid, &status, 0) < 0) {
      write(STDERR_FILENO, FORMAT_MSG("shell", WAIT_ERROR_MSG),
            strlen(FORMAT_MSG("shell", WAIT_ERROR_MSG)));
    }
  }
}

// Ccollect terminated background processes
void collect_background(void) {
  int status;
  while (waitpid(-1, &status, WNOHANG) > 0) {
  }
}
