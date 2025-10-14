#define POSIX_C_SOURCE 200809L
#include "../include/task1.h"
#include "../include/msgs.h"
#include "../include/task0.h"
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// keep a track of the old directory
static char prev_dir[INPUT_SIZE] = "";

// helper to get the user's home directory
static char *get_home_dir(void) {
  struct passwd *user = getpwuid(getuid());
  return user ? user->pw_dir : NULL;
}

static void save_current_dir(void) {
  char temp[INPUT_SIZE];
  if (getcwd(temp, sizeof(temp))) {
    strncpy(prev_dir, temp, sizeof(prev_dir) - 1);
    prev_dir[sizeof(prev_dir) - 1] = '\0';
  }
}

// expand the tilde path
static int expand_tilde_path(const char *path, char *output, size_t size) {
  if (path[0] != '~') {
    strncpy(output, path, size - 1);
    output[size - 1] = '\0';
    return 1;
  }

  char *home = get_home_dir();
  if (!home)
    return 0;

  if (path[1] == '\0') {
    strncpy(output, home, size - 1);
  } else if (path[1] == '/') {
    size_t home_len = strlen(home);
    size_t path_len = strlen(path + 1);
    if (home_len + path_len >= size)
      return 0;

    strcpy(output, home);
    strcat(output, path + 1);
  } else {
    return 0;
  }

  output[size - 1] = '\0';
  return 1;
}

// show help command
static void print_all_help(void) {
  write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
        strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
        strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
        strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
        strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("historY", HISTORY_HELP_MSG),
        strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
}

// show help for specific command
static void print_command_help(const char *cmd) {
  if (strcmp(cmd, "help") == 0) {
    write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
          strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
  } else if (strcmp(cmd, "exit") == 0) {
    write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
          strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
  } else if (strcmp(cmd, "pwd") == 0) {
    write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
          strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
  } else if (strcmp(cmd, "cd") == 0) {
    write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
          strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
  } else if (strcmp(cmd, "history") == 0) {
    write(STDOUT_FILENO, FORMAT_MSG("history", HISTORY_HELP_MSG),
          strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
  } else {
    write(STDOUT_FILENO, cmd, strlen(cmd));
    write(STDOUT_FILENO, ": " EXTERN_HELP_MSG "\n",
          strlen(": " EXTERN_HELP_MSG "\n"));
  }
}

int is_internal(const char *cmd) {
  if (!cmd)
    return 0;

  return strcmp(cmd, "exit") == 0 || strcmp(cmd, "pwd") == 0 ||
         strcmp(cmd, "cd") == 0 || strcmp(cmd, "help") == 0 ||
         strcmp(cmd, "history") == 0;
}

// exit command
void run_exit(char **args) {
  if (args[1]) {
    write(STDERR_FILENO, FORMAT_MSG("exit", TMA_MSG),
          strlen(FORMAT_MSG("exit", TMA_MSG)));
  } else {
    exit(0);
  }
}

// pwd command
void run_pwd(char **args) {
  if (args[1]) {
    write(STDERR_FILENO, FORMAT_MSG("pwd", TMA_MSG),
          strlen(FORMAT_MSG("pwd", TMA_MSG)));
    return;
  }

  char dir[INPUT_SIZE];
  if (getcwd(dir, sizeof(dir))) {
    write(STDOUT_FILENO, dir, strlen(dir));
    write(STDOUT_FILENO, "\n", 1);
  } else {
    write(STDERR_FILENO, FORMAT_MSG("pwd", GETCWD_ERROR_MSG),
          strlen(FORMAT_MSG("pwd", GETCWD_ERROR_MSG)));
  }
}

// CD command
void run_cd(char **args) {
  char target[INPUT_SIZE];

  // toomany arguments
  if (args[1] && args[2]) {
    write(STDERR_FILENO, FORMAT_MSG("cd", TMA_MSG),
          strlen(FORMAT_MSG("cd", TMA_MSG)));
    return;
  }

  // save the cwd before changing
  save_current_dir();

  // determine the target directory
  if (!args[1]) {
    char *home = get_home_dir(); // no argument : home
    if (!home) {
      write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
            strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      return;
    }
    strncpy(target, home, sizeof(target) - 1);
  } else if (strcmp(args[1], "-") == 0) {
    // previous directory
    if (prev_dir[0] == '\0') {
      write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
            strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      return;
    }
    strncpy(target, prev_dir, sizeof(target) - 1);
  } else if (args[1][0] == '~') {
    if (!expand_tilde_path(args[1], target, sizeof(target))) {
      write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
            strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      return;
    }
  } else {
    // regular path
    strncpy(target, args[1], sizeof(target) - 1);
  }

  target[sizeof(target) - 1] = '\0';

  // attempt directory change
  if (chdir(target) != 0) {
    write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
          strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
    return;
  }

  if (!args[1]) {
    print_all_help();
  } else {
    print_command_help(args[1]);
  }
}

void run_help(char **args) {
  if (args[1] && args[2]) {
    write(STDERR_FILENO, FORMAT_MSG("help", TMA_MSG),
          strlen(FORMAT_MSG("help", TMA_MSG)));
    return;
  }

  if (!args[1]) {
    print_all_help();
  } else {
    print_command_help(args[1]);
  }
}

// SIGINT handler
void handle_sigint(int sig) {
  (void)sig;

  write(STDOUT_FILENO, "\n", 1);

  write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
        strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
        strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
        strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
        strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("historY", HISTORY_HELP_MSG),
        strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
}

// setup function
void setup_shell(void) { signal(SIGINT, handle_sigint); }
